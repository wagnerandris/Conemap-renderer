#ifndef GUI_HPP
#define GUI_HPP

// STD
#include <algorithm>
#include <filesystem>
#include <string>

// ImGui
#include <imgui.h>
#include <external/imgui-filebrowser/imfilebrowser.h>

#include "file_utils.hpp"
#include "ConeSteppingObject.hpp"

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

	ImGui::FileBrowser fileDialog;

	TextureResourceSelect(std::string label_, GLuint &selected_id_)
			: label(label_), selected_id(selected_id_) {}

	~TextureResourceSelect() {
		for (auto res : resources) {
			glDeleteTextures(1, &res.id);
		}
	}

	void load_file(std::filesystem::path path) {
		auto it = std::find_if(resources.begin(), resources.end(),
													 [&path](const TextureResource &res) {
														 return std::filesystem::equivalent(res.path, path);
													 });

		if (it != resources.end()) {
			error = "Already loaded.";
			return;
		}

		GLuint id = load_texture_from_file(path);
		
		if(!id) {
			error = "Could not load.";
			return;
		}

		resources.push_back(TextureResource{path, path.filename(), id});
		selected_index = resources.size() - 1;
		selected_id = resources[selected_index].id;
		error = ""; // remove error message after succesfully loading an image
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
			fileDialog.Open();
		}
		ImGui::PopID();

		fileDialog.Display();

		if (fileDialog.HasSelected()) {
			std::filesystem::path path = fileDialog.GetSelected();
			fileDialog.ClearSelected();

			load_file(path);
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

public:
	Gui(int &steps_, int &display_mode_, bool &show_convergence_,
			ConeSteppingObject &object_, 
			std::vector<std::string> &input_cone_maps,
			std::vector<std::string> &input_textures) :
				steps(steps_),
				display_mode(display_mode_),
				show_convergence(show_convergence_),
				object(object_),
				cone_maps(TextureResourceSelect("Cone map", object.stepmapTex)),
				textures(TextureResourceSelect("Texture", object.texmapTex)),
				depth(object.depth)
	{
		// load conemaps and textures passed as arguments
		for (std::filesystem::path path : input_cone_maps) {
			cone_maps.load_file(path);
		}
		for (std::filesystem::path path : input_textures) {
			textures.load_file(path);
		}
	}

	void compose(double fps) {
		// Rendering
		if (ImGui::Begin("Rendering")) {
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
