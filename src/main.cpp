// GLEW
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// GLM
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
    if(ImGui::Begin("dummy window"))
      {
        // open file dialog when user clicks this button
				if(ImGui::Button("open file dialog"))
					fileDialog.Open();
			}
			ImGui::End();
		
			fileDialog.Display();
		
			if(fileDialog.HasSelected())
			{
				printf("Selected filename %s\n", fileDialog.GetSelected().c_str());
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
