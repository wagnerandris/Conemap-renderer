#ifndef GUI_HPP
#define GUI_HPP

// STD
#include <algorithm>
#include <filesystem>
#include <mutex>
#include <optional>
#include <queue>
#include <string>
#include <thread>

// ImGui
#include "external/imgui/src/imgui.h"
#include "external/imgui-filebrowser/imfilebrowser.h"

#include "file_utils.hpp"
#include "ConeSteppingObject.hpp"

// Cone map generation
#include "subprojects/Conemap/src/conemap.hpp"

struct TextureResource {
	std::filesystem::path path;
	std::string name;
	GLuint id = 0;
};

struct TextureResourceSelect {
	std::string label;
	std::string error = "";

	std::vector<TextureResource> resources;
	unsigned int selected_index = 0;
	GLuint &selected_id;

	ImGui::FileBrowser file_selector = ImGui::FileBrowser(
			ImGuiFileBrowserFlags_MultipleSelection |
			ImGuiFileBrowserFlags_ConfirmOnEnter |
			ImGuiFileBrowserFlags_EditPathString
			);

	TextureResourceSelect(const std::string &label_, GLuint &selected_id_)
			: label(label_), selected_id(selected_id_) {}

	TextureResourceSelect(const std::string &label_, GLuint &selected_id_, const std::vector<std::filesystem::path> &paths)
			: TextureResourceSelect(label_, selected_id_) {
				load_files(paths);
				
				// select the last file loaded
				if (!resources.empty()) {
					selected_index = resources.size() - 1;
					selected_id = resources[selected_index].id;
				}
			}

	~TextureResourceSelect() {
		for (auto res : resources) {
			glDeleteTextures(1, &res.id);
		}
	}

	bool load_files(const std::vector<std::filesystem::path> &paths) {
		error = ""; // remove error messages before attempting to load new textures

		bool success = false;
		for (std::filesystem::path path : paths) {
			auto it = std::find_if(resources.begin(), resources.end(),
													 	 [&path](const TextureResource &res) {
														 	 return std::filesystem::equivalent(res.path, path);
													 	 });

			if (it != resources.end()) {
				error += path.string() + " is already loaded.\n";
				continue;
			}

			GLuint id = load_texture_from_file(path);
			
			if(!id) {
				error += path.string() + " could not be loaded.\n";
				continue;
			}

			resources.push_back(TextureResource{path, path.filename(), id});

			success = true;
		}

		return success;
	}

	void file_combo() {
		const char *combo_preview_value =
				selected_index < resources.size() // if there is a selected resource
						? resources[selected_index].name.c_str() // display its name
						: "";

		if (ImGui::BeginCombo(label.c_str(), combo_preview_value)) {
			static ImGuiTextFilter filter;
			if (ImGui::IsWindowAppearing()) {
				ImGui::SetKeyboardFocusHere();
				filter.Clear();
			}
			ImGui::SetNextItemShortcut(ImGuiMod_Ctrl | ImGuiKey_F);
			filter.Draw("##Filter", -FLT_MIN);

			for (size_t i = 0; i < resources.size(); i++) {
				const bool is_selected = (selected_index == i);

				if (filter.PassFilter(resources[i].name.c_str())) { // only display names that match the filer string
					ImGui::PushID(resources[i].id); // needed for resources with the same name
					if (ImGui::Selectable(resources[i].name.c_str(), is_selected)) {
						// set selected
						selected_index = i;
						selected_id = resources[selected_index].id;
						error = ""; // remove error when selecting new texture
					}
					ImGui::PopID();
				}

				if (ImGui::IsItemHovered())
					ImGui::SetTooltip("%s", resources[i].path.c_str());
			}
			ImGui::EndCombo();
		}
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("%s", selected_index < resources.size()
																	? resources[selected_index].path.c_str()
																	: "");

		ImGui::PushID(&label); // needed for multiple buttons with the same label
		if (ImGui::Button("Add new")) {
			file_selector.Open();
		}
		ImGui::PopID();

		file_selector.Display();

		if (file_selector.HasSelected()) {
			std::vector<std::filesystem::path> input_files = file_selector.GetMultiSelected();
			file_selector.ClearSelected();

			if (load_files(input_files)) {
				// select the last file loaded
				selected_index = resources.size() - 1;
				selected_id = resources[selected_index].id;
			}
		}

		ImGui::TextColored(ImVec4(1, 0, 0, 1), // red
											"%s", error.c_str());
	}
};


template <typename T>
class ThreadSafeQueue {
	std::queue<T> q;
	std::mutex m;

public:
	void push(T v) {
		std::lock_guard<std::mutex> lock(m);
		q.push(std::move(v));
	}

	std::optional<T> try_pop() {
		std::lock_guard<std::mutex> lock(m);
		if (q.empty()) return std::nullopt;
		T v = std::move(q.front());
		q.pop();
		return v;
	}

	bool empty() {
		std::lock_guard<std::mutex> lock(m);
		return q.empty();
	}
};

class ConeMapGenerator {
	ThreadSafeQueue<std::filesystem::path> input_queue;
	ThreadSafeQueue<std::filesystem::path> output_queue;

	std::counting_semaphore<> sem{0}; // initially 0
	std::jthread worker;
	std::atomic<bool> processing;

	std::filesystem::path output_path = std::filesystem::current_path();

	ImGui::FileBrowser directory_selector = ImGui::FileBrowser(
			ImGuiFileBrowserFlags_SelectDirectory |
			ImGuiFileBrowserFlags_HideRegularFiles |
			ImGuiFileBrowserFlags_ConfirmOnEnter |
			ImGuiFileBrowserFlags_EditPathString
			);

	ImGui::FileBrowser file_selector = ImGui::FileBrowser(
			ImGuiFileBrowserFlags_MultipleSelection |
			ImGuiFileBrowserFlags_ConfirmOnEnter |
			ImGuiFileBrowserFlags_EditPathString
			);

public:
	ConeMapGenerator() {
		file_selector.SetTypeFilters({".png", ".jpg", ".jpeg"});
		// TODO file selector names

		// start worker thread
		worker = std::jthread([this](std::stop_token stoken) {
			while (!stoken.stop_requested()) {
				// wait until at least one input is queued
				sem.acquire();

				// check again, in case stop was requested while waiting
				if (stoken.stop_requested()) break;

				std::optional<std::filesystem::path> input = input_queue.try_pop();

				processing.store(true);

				std::filesystem::path output = conemap::discrete(output_path, *input);

				// Push result
				output_queue.push(output);

				processing.store(false);
			}
		});
	}

	~ConeMapGenerator() {
		worker.request_stop();
		sem.release();
	}

	std::filesystem::path compose() {
		ImGui::TextWrapped("Cone map generation output directory:\n%s", output_path.c_str());
		if (ImGui::Button("Change")) {
			directory_selector.Open();
		}
		directory_selector.Display();
		
		if (directory_selector.HasSelected()) {
			output_path = directory_selector.GetSelected();
			directory_selector.ClearSelected();
		}

		// TODO discrete/analytic
		// TODO wrapping texture
		if (ImGui::Button("Generate cone map from texture")) {
			file_selector.Open();
		}

		file_selector.Display();

		std::vector<std::filesystem::path> input_files;
		if (file_selector.HasSelected()) {
			input_files = file_selector.GetMultiSelected();
			file_selector.ClearSelected();

			for (std::filesystem::path input : input_files) {
				input_queue.push(input);
				// signal that an input can be processed
				sem.release();
			}
		}

		std::optional<std::filesystem::path> output = output_queue.try_pop();

		ImGui::TextWrapped("%s", processing.load() ? "Generating in progress" : "");

		if (output) {
			return *output;
		}
		return "";
	}
};

// TODO give titles to file dialogue windows
class Gui {
	// rendering settings
	int &steps;
	int &display_mode;
	bool &show_convergence;

	// object settings
	ConeSteppingObject &object;
	TextureResourceSelect cone_maps;
	TextureResourceSelect textures;
	float &depth;

	// cone map generation
	ConeMapGenerator cone_map_generator;

public:
	Gui(int &steps_, int &display_mode_, bool &show_convergence_,
			ConeSteppingObject &object_, 
			std::vector<std::filesystem::path> &input_cone_maps,
			std::vector<std::filesystem::path> &input_textures) :
				steps(steps_),
				display_mode(display_mode_),
				show_convergence(show_convergence_),
				object(object_),
				cone_maps(TextureResourceSelect("Cone map", object.stepmapTex, input_cone_maps)),
				textures(TextureResourceSelect("Texture", object.texmapTex, input_textures)),
				depth(object.depth),
				cone_map_generator(ConeMapGenerator()) {}

	void compose(double fps) {
		// Rendering
		if (ImGui::Begin("Rendering")) {
			std::filesystem::path new_cone_map = cone_map_generator.compose();
			if (!new_cone_map.empty()) {
				cone_maps.load_files({new_cone_map});
			}
			cone_maps.file_combo();
			ImGui::SliderFloat("Depth", &depth, 0.1f, 1.0f);
			ImGui::SliderInt("Binary search steps", &steps, 0, 16);
			ImGui::Checkbox("Show convergence", &show_convergence);

			ImGui::RadioButton("Heights", &display_mode, 1);
			ImGui::RadioButton("Cones", &display_mode, 2);
			ImGui::RadioButton("Normals", &display_mode, 3);
			ImGui::RadioButton("Color texture", &display_mode, 0);
				ImGui::BeginDisabled(display_mode);
					textures.file_combo(); // only active in Color texture display mode
				ImGui::EndDisabled();
		}
		ImGui::End();

		if (ImGui::Begin("FPS")) {
			ImGui::LabelText("", "%f", fps);
		}
		ImGui::End();
	}
};

#endif
