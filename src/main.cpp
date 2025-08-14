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
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <imgui.h>

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

  /* Start ImGui*/
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init();

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


  /* Loop until the user closes the window */
  while (!glfwWindowShouldClose(window)) {

    /* Render frame */
    glClearColor(0.1f, 0.2f, 0.6f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // TODO to render code?
    scene->render();

    /* Render ImGui */
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame(); // After this ImGui commands can be called until
                               // ImGui::Render()

    ImGui::NewFrame();
    /* Compose ImGui here */
    ImGui::ShowDemoWindow(); // TODO delete
    ImGui::Render();

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    /* Swap front and back buffers */
    glfwSwapBuffers(window);
    
    /* Poll for and process events */

		// see if imgui captures events
		keyboard_to_imgui = ImGui::GetIO().WantCaptureKeyboard;
		mouse_to_imgui = ImGui::GetIO().WantCaptureMouse;

    glfwPollEvents();
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
