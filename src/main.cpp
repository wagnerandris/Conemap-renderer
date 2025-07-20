// GLEW
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// ImGui
#include <cstdio>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

static void error_callback(int error, const char *description) {
  fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action,
                         int mods) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GLFW_TRUE);
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

  /* Setup key callback */
  glfwSetKeyCallback(window, key_callback);

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

  /* Loop until the user closes the window */
  while (!glfwWindowShouldClose(window)) {
    /* Get window size and ratio */
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    // const float ratio = width / (float)height;

    glViewport(0, 0, width, height);

    /* Render frame */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // TODO to render code
    // TODO render here
	
		/* Render ImGui */
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame(); // After this ImGui commands can be called until ImGui::Render()

		ImGui::NewFrame();
		/* Compose ImGui here */
		ImGui::ShowDemoWindow(); // TODO delete
		ImGui::Render();

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


    /* Swap front and back buffers */
    glfwSwapBuffers(window);

    /* Poll for and process events */
    glfwPollEvents();
  }

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
