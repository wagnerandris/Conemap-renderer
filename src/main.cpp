// GLEW
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// GLM
#include <cstddef>
#include <filesystem>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>

// ImGui
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <external/imgui-filebrowser/imfilebrowser.h>

// STD
#include <cstdio>

#include "scene.hpp"


static Scene* scene;
static bool keyboard_to_imgui;
static bool mouse_to_imgui;

static void error_callback(int error, const char *description) {
  fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action,
                         int mods) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GLFW_TRUE);

  // TODO keybinds
	if (!keyboard_to_imgui) {
		scene->controls.keyboard_action(key, scancode, action, mods);
	}
}

static void mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {
	if (!mouse_to_imgui) {
		scene->controls.mouse_button_action(button, action, mods);
	}
}

static double xpos;
static double ypos;
static void cursor_pos_callback(GLFWwindow *window, double _xpos, double _ypos) {
	if (!mouse_to_imgui) {
		
		scene->controls.mouse_move_action(_xpos - xpos, _ypos - ypos);
		xpos = _xpos;
		ypos = _ypos;
	}
}

static void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
	if (!mouse_to_imgui) {
		scene->controls.mouse_scroll_action(xoffset, yoffset);
	}
}

static void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
	// set viewport
  glViewport(0, 0, width, height);

  /// set camera aspect ratio
	scene->controls.set_aspect(width / (float)height);
	
}

// TODO separate file
void file_combo(const std::vector<std::string> names, unsigned int& selected_idx, const std::string label) {
	const char* combo_preview_value = names[selected_idx].c_str();
	if (ImGui::BeginCombo(label.c_str(), combo_preview_value))
	{
    	static ImGuiTextFilter filter;
    	if (ImGui::IsWindowAppearing())
    	{
        	ImGui::SetKeyboardFocusHere();
        	filter.Clear();
    	}
    	ImGui::SetNextItemShortcut(ImGuiMod_Ctrl | ImGuiKey_F);
    	filter.Draw("##Filter", -FLT_MIN);

    	for (size_t i = 0; i < names.size(); i++)
    	{
        	const bool is_selected = (selected_idx == i);
        	if (filter.PassFilter(names[i].c_str()))
            	if (ImGui::Selectable(names[i].c_str(), is_selected))
                	selected_idx = i;
    	}
    	ImGui::EndCombo();
	}
}

int main(void) {
  /* Setup error callback */
  glfwSetErrorCallback(error_callback);

  GLFWwindow *window;

  /* Initialize the library */
  if (!glfwInit())
    return -1;

  /* Create a windowed mode window and its OpenGL context */
  window = glfwCreateWindow(640, 480, "Conemap renderer", NULL, NULL);
  if (!window) {
    glfwTerminate();
    return -1;
  }

  /* Make the window's context current */
  glfwMakeContextCurrent(window);

  /* Start GLEW */
  GLenum error = glewInit();
  if (error != GLEW_OK) {
    fprintf(stderr, "Error during the initialization of glew.");
    return -1;
  }
  
  /* Set vsync */
  glfwSwapInterval(1);

  /* Create scene */
  scene = new Scene();
  
	/* Setup input callbacks */
	// only after scene creation as these relay input to scene
	glfwGetCursorPos(window, &xpos, &ypos);
  glfwSetKeyCallback(window, key_callback);
  glfwSetMouseButtonCallback(window, mouse_button_callback);
  glfwSetCursorPosCallback(window, cursor_pos_callback);
  glfwSetScrollCallback(window, scroll_callback);

  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  /* Start ImGui*/
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init();

  // create a file browser instance
  ImGui::FileBrowser fileDialog;

	static double elapsed_time = glfwGetTime();

  /* Loop until the user closes the window */
  while (!glfwWindowShouldClose(window)) {
  	// calculate elapsed time
		double now = glfwGetTime();
		double delta_time = now - elapsed_time;
		elapsed_time = now;

  /* Poll for and process events */

		// see if imgui captures events
		keyboard_to_imgui = io.WantCaptureKeyboard;
		mouse_to_imgui = io.WantCaptureMouse;

		// execute callback functions as needed
    glfwPollEvents();

  /* Render ImGui */
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame(); // After this ImGui commands can be called until
                               // ImGui::Render()

    ImGui::NewFrame();

  /* Compose ImGui here */
  	static std::vector<std::filesystem::path>* path_vector;
  	static std::vector<std::string>* name_vector;
		static std::vector<std::filesystem::path> cone_maps = {"pyramids_cone_map.png"};
		static std::vector<std::string> cone_map_names = {cone_maps[0].c_str()};
		static unsigned int selected_cone_map_idx = 0;
		static std::vector<std::filesystem::path> textures = {"wood.png"};
		static std::vector<std::string> texture_names = {cone_maps[0].c_str()};
		static unsigned int selected_texture_idx = 0;
    if(ImGui::Begin("Textures"))
      {
      	file_combo(cone_map_names, selected_cone_map_idx, "cone map");
				if(ImGui::Button("Add cone map")) { // TODO put these in the fuction as well (only makes sense in separate file/struct)
					path_vector = &cone_maps;
					name_vector = &cone_map_names;
					fileDialog.Open();
				}
      	file_combo(texture_names, selected_texture_idx, "texture");
				if(ImGui::Button("Add texture")) {
					path_vector = &textures;
					name_vector = &texture_names;
					fileDialog.Open();
				}
			}
			ImGui::End();
		
			fileDialog.Display();
		
			if(fileDialog.HasSelected())
			{
				path_vector->push_back(fileDialog.GetSelected());
				name_vector->push_back(path_vector->back().filename().string());
				fileDialog.ClearSelected();
			}
		ImGui::ShowDemoWindow(); // TODO delete
		ImGui::Render();

  /* Render frame */
		// move camera if key is being pressed
    scene->controls.move(delta_time);

		// render
    scene->render();

		// render ImGui on top
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

  /* Swap front and back buffers */
    glfwSwapBuffers(window);
  }

	/* Delete scene */
	delete scene;

  /* Terminate ImGui */
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  /* Terminate GLFW */
  if (window != NULL)
    glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}
