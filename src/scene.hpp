#ifndef SCENE_HPP
#define SCENE_HPP

// GLEW
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// GLM
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "ConeSteppingObject.hpp"

class Scene {

  // program
  GLuint program;
  void create_shaders();

  // textures
  GLuint stepmapTexID;
  GLuint texmapTexID;
  void load_textures();

  // camera
  glm::mat4 viewMatrix =
      glm::lookAt(glm::vec3(0.0f, 1.0f, 1.0f), glm::vec3(0.0f),
                  glm::vec3(0.0f, 0.0f, 1.0f));

  float ratio = 16.0 / 9.0; // TODO window change callback
  glm::mat4 projectionMatrix =
      glm::perspective(glm::half_pi<float>(), ratio, 0.0f, 1000.0f);

  // lights
  glm::vec3 light_pos = glm::vec3(0.0f, 0.0f, 1.0f);

  // objects
  ConeSteppingObject* quad;
  void create_scene_objects();

public:
	Scene();
	~Scene();
  // rendering
  void render();
};
#endif
