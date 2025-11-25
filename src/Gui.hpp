#ifndef GUI_HPP
#define GUI_HPP

// STD
#include <algorithm>
#include <filesystem>
#include <string>

// ImGui
#include "external/imgui/src/imgui.h"
#include "external/imgui-filebrowser/imfilebrowser.h"


#include "file_utils.hpp"
#include "ConeSteppingObject.hpp"
#include "ConeMapGenerator.hpp"

struct TextureResource {
	std::filesystem::path path;
	std::string name;
	GLuint id = 0;
};

struct TextureResourceSelect {
	std::string label;
	std::string error = "";

	std::vector<TextureResource> resources;
	int selected_index = -1;
	GLuint &selected_id;

	ImGui::FileBrowser file_selector = ImGui::FileBrowser(
			ImGuiFileBrowserFlags_MultipleSelection |
			ImGuiFileBrowserFlags_ConfirmOnEnter |
			ImGuiFileBrowserFlags_EditPathString
			);

	bool require_conemap;

	TextureResourceSelect(const std::string &label_, GLuint &selected_id_, bool require_conemap_ = false)
			: label(label_), selected_id(selected_id_), require_conemap(require_conemap_) {
				file_selector.SetTypeFilters({".png", ".jpg", ".jpeg"});
				file_selector.SetTitle(label + " selection");
			}

	TextureResourceSelect(const std::string &label_, GLuint &selected_id_, const std::vector<std::filesystem::path> &paths, bool require_conemap_ = false)
			: TextureResourceSelect(label_, selected_id_, require_conemap_) {
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

			GLuint id = load_texture_from_file(path, require_conemap);
			
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
				selected_index >= 0 // selected_index < resources.size() // if there is a selected resource
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
				const bool is_selected = (static_cast<size_t>(selected_index) == i);

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
			ImGui::SetTooltip("%s", selected_index >= 0 // selected_index < resources.size()
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
				cone_maps(TextureResourceSelect("Cone map", object.stepmapTex, input_cone_maps, true)),
				textures(TextureResourceSelect("Texture", object.texmapTex, input_textures)),
				depth(object.depth),
				cone_map_generator(ConeMapGenerator()) {}

	void compose(double fps) {
		// Cone map generation
		if (ImGui::Begin("Cone map generation")) {
			std::filesystem::path new_cone_map = cone_map_generator.compose();
			if (!new_cone_map.empty()) {
				cone_maps.load_files({new_cone_map});
			}
		}
		ImGui::End();

		// Rendering
		if (ImGui::Begin("Rendering")) {
			cone_maps.file_combo();
			ImGui::SliderFloat("Depth", &depth, 0.0f, 1.0f);
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

		// FPS counter
		if (ImGui::Begin("FPS")) {
			ImGui::LabelText("", "%f", fps);
		}
		ImGui::End();
	}
};

#endif
