// GLEW
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// GLM
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <string>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>

// ImGui
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <imgui.h>

// STD
#include <filesystem>
#include <fstream>
#include <vector>

static std::string read_file(const std::filesystem::path &path) {
  // open file and seek to the end
  std::ifstream file(path, std::ios::in | std::ios::binary | std::ios::ate);
  if (!file.is_open())
    fprintf(stderr, "Error: Could not open %s\n", path.c_str());

  std::ifstream::pos_type bytesize = file.tellg();
  if (bytesize == std::ifstream::pos_type(-1))
    fprintf(stderr, "Error: Could not read %s\n", path.c_str());

  file.seekg(0, std::ios::beg); // reset cursor to beginning

  std::vector<char> bytes(bytesize);
  file.read(bytes.data(), bytesize);

  return std::string(bytes.data(), bytesize);
}

static void load_shader_from_file(const GLuint shader, const std::filesystem::path &path) {
  const std::string shader_string = read_file(path);
  const char *shader_text = shader_string.data();
  const GLint shader_length = shader_string.length();
  glShaderSource(shader, 1, &shader_text, &shader_length);
  glCompileShader(shader);
}

// TODO delete start

typedef struct Vertex {
  glm::vec2 pos;
  glm::vec3 col;
} Vertex;

static const Vertex vertices[3] = {{{-0.6f, -0.4f}, {1.f, 0.f, 0.f}},
                                   {{0.6f, -0.4f}, {0.f, 1.f, 0.f}},
                                   {{0.f, 0.6f}, {0.f, 0.f, 1.f}}};
// TODO delete end

static void error_callback(int error, const char *description) {
  fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action,
                         int mods) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GLFW_TRUE);

  // TODO keybinds
}

// TODO mouse
// check ImGui capture?

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

  // /* Init shader programs */

  // TODO delete start
  GLuint vertex_buffer;
  glGenBuffers(1, &vertex_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  const GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  load_shader_from_file(vertex_shader, "test.vert");

  const GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  load_shader_from_file(fragment_shader, "test.frag");

  const GLuint program = glCreateProgram();
  glAttachShader(program, vertex_shader);
  glAttachShader(program, fragment_shader);
  glLinkProgram(program);

  const GLint mvp_location = glGetUniformLocation(program, "MVP");
  const GLint vpos_location = glGetAttribLocation(program, "vPos");
  const GLint vcol_location = glGetAttribLocation(program, "vCol");

  GLuint vertex_array;
  glGenVertexArrays(1, &vertex_array);
  glBindVertexArray(vertex_array);
  glEnableVertexAttribArray(vpos_location);
  glVertexAttribPointer(vpos_location, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, pos));
  glEnableVertexAttribArray(vcol_location);
  glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, col));
  // TODO delete end

  /* Set vsync */
  glfwSwapInterval(1);

  /* Loop until the user closes the window */
  while (!glfwWindowShouldClose(window)) {
    // TODO to window change callback
    /* Get window size and ratio */
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    const float ratio = width / (float)height;

    glViewport(0, 0, width, height);

    /* Render frame */
    glClearColor(0.1f, 0.3f, 0.8f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // TODO to render code
    // TODO render here

    // TODO delete srart
    glm::mat4 world =
        glm::rotate((float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));

    // TODO to window change callback
    glm::mat4 view_projection =
        glm::perspective(glm::half_pi<float>(), ratio, 0.0f, 1000.0f) *
        glm::lookAt(glm::vec3(0.0f, 1.0f, 1.0f), glm::vec3(0.0f),
                    glm::vec3(0.0f, 0.0f, 1.0f));

    glm::mat4 mvp = view_projection * world;

    glUseProgram(program);
    glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat *)&mvp);
    glBindVertexArray(vertex_array);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    // TODO delete end

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
