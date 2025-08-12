#ifndef SCENE_HPP
#define SCENE_HPP

// Utils
#include "ConeSteppingObject.hpp"
#include "Camera.hpp"

class Scene {

  // program
  GLuint program;
  void create_shaders();

  // textures
  GLuint stepmapTexID;
  GLuint texmapTexID;
  void load_textures();

  // camera
  Camera camera;

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
