#ifndef GUI_HPP
#define GUI_HPP

#include "file_utils.hpp"

#include <filesystem>
#include <imgui.h>
#include <external/imgui-filebrowser/imfilebrowser.h>

#include <algorithm>

struct Resource {
  std::filesystem::path path;
  std::string name;
  GLuint id = 0;
};

struct ResourceSelect {
  std::string label;

  std::vector<Resource> resources;
  unsigned int selected_index = 0;
  GLuint &selected_id;
  GLuint (*load_func)(std::filesystem::path);

  ImGui::FileBrowser fileDialog;

  ResourceSelect(std::string label_, GLuint& selected_id_, GLuint (*load_func_)(std::filesystem::path)) : label(label_), selected_id(selected_id_), load_func(load_func_) {}

  void load_file(std::filesystem::path path) {
    // TODO popup error messages?
    auto it = std::find_if(resources.begin(), resources.end(),
                           [&path](const Resource &res) {
                             return std::filesystem::equivalent(res.path, path);
                           });

    if (it == resources.end()) {
      GLuint id = load_func(path);
      if (id) {
        resources.push_back(Resource{path, path.filename(), id});
        selected_index = resources.size() - 1;
        selected_id = resources[selected_index].id;
      }
    }
  }

  void file_combo() {
    const char *combo_preview_value =
        selected_index < resources.size()
            ? resources[selected_index].name.c_str()
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

        if (filter.PassFilter(resources[i].name.c_str()))
          if (ImGui::Selectable(resources[i].name.c_str(), is_selected)) {
            selected_index = i;
						selected_id = resources[selected_index].id;
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

      // TODO popup error messages?
    }
  }
};

class Gui {
  ResourceSelect cone_maps;
  ResourceSelect textures;
  // ResourceSelect models; TODO

public:
  Gui(GLuint &cone_map_id, GLuint &texture_id) :
    cone_maps(ResourceSelect("Cone map", cone_map_id,
                               [](std::filesystem::path path) {
                                 return load_texture_from_file(path.c_str());
                               })),
    textures(ResourceSelect("Texture", texture_id,
                               [](std::filesystem::path path) {
                                 return load_texture_from_file(path.c_str());
                               }))
    {}

  void compose() {
    texture_select();
    // model_select(); TODO
  }

private:
  void texture_select() {
    if (ImGui::Begin("Textures")) {
      cone_maps.file_combo();
      textures.file_combo();
    }
    ImGui::End();
  }
};

#endif
