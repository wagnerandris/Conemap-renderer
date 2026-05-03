#ifndef GUI_HPP
#define GUI_HPP

// STD
#include <filesystem>
#include <string>

// ImGui
#include "imgui.h"
#include "subprojects/imgui-filebrowser/imfilebrowser.h"


#include "file_utils.hpp"
#include "ConeSteppingObject.hpp"
#include "ConeMapGenerator.hpp"

struct TextureResourceSelect {
	std::string label;
	std::string error = "";

	std::vector<TextureResource> &resources;
	int selected_index = -1;
	GLuint &selected_id;

	ImGui::FileBrowser file_selector = ImGui::FileBrowser(
			ImGuiFileBrowserFlags_MultipleSelection |
			ImGuiFileBrowserFlags_ConfirmOnEnter |
			ImGuiFileBrowserFlags_EditPathString
			);

	bool require_conemap;

	TextureResourceSelect(const std::string &label_, std::vector<TextureResource> &resources_, GLuint &selected_id_, bool require_conemap_ = false)
			: label(label_), resources(resources_), selected_id(selected_id_), require_conemap(require_conemap_) {
				file_selector.SetTypeFilters({".png", ".jpg", ".jpeg"});
				file_selector.SetTitle(label + " selection");
			}

	TextureResourceSelect(const std::string &label_, std::vector<TextureResource> &resources_, GLuint &selected_id_, const std::vector<std::filesystem::path> &paths, bool require_conemap_ = false)
			: TextureResourceSelect(label_, resources_, selected_id_, require_conemap_) {
				error = load_textures(paths, resources, require_conemap);
				
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

			size_t prev_res_num = resources.size();
			error = load_textures(input_files, resources, require_conemap);
			if (prev_res_num != resources.size()) {
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
	int &cone_steps;
	int &binary_steps;
	int &display_mode;
	bool &cell_max_trace;
	bool &show_convergence;

	// object settings
	ConeSteppingObject &object;
	TextureResourceSelect cone_maps;
	TextureResourceSelect textures;
	float &depth;

	// cone map generation
	ConeMapGenerator cone_map_generator;

public:
	Gui(int &cone_steps_, int &binary_steps_, int &display_mode_, bool &cell_max_trace_, bool &show_convergence_,
			ConeSteppingObject &object_, 
			std::vector<TextureResource> &conemap_resources,
			std::vector<TextureResource> &texmap_resources,
			std::vector<std::filesystem::path> &input_cone_maps,
			std::vector<std::filesystem::path> &input_textures) :
				cone_steps(cone_steps_),
				binary_steps(binary_steps_),
				display_mode(display_mode_),
				cell_max_trace(cell_max_trace_),
				show_convergence(show_convergence_),
				object(object_),
				cone_maps(TextureResourceSelect("Cone map", conemap_resources, object.conemapTex, input_cone_maps, true)),
				textures(TextureResourceSelect("Texture", texmap_resources, object.texmapTex, input_textures)),
				depth(object.depth),
				cone_map_generator(ConeMapGenerator()) {}

	void compose(double fps) {
		// Cone map generation
		if (ImGui::Begin("Cone map generation")) {
			std::filesystem::path new_cone_map = cone_map_generator.compose();
			if (!new_cone_map.empty()) {
				cone_maps.error = load_textures({new_cone_map}, cone_maps.resources, cone_maps.require_conemap);
			}
		}
		ImGui::End();

		// Rendering
		if (ImGui::Begin("Rendering")) {
			cone_maps.file_combo();
			ImGui::SliderFloat("Depth", &depth, 0.0f, 1.0f);
			ImGui::SliderInt("Cone steps", &cone_steps, 0, 256);
			ImGui::SliderInt("Binary search steps", &binary_steps, 0, 16);
			ImGui::Checkbox("Cell-max tracing", &cell_max_trace);
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
