#ifndef SCENE_HPP
#define SCENE_HPP

// Utils
#include "ConeSteppingObject.hpp"
#include "Camera.hpp"
#include "Controls.hpp"

class Scene {

  // program
  GLuint program;
  void create_shaders();

  // camera
  Camera camera;

  // lights
  glm::vec3 light_pos = glm::vec3(0.0f, 1.0f, 0.0f);

  // objects
  ConeSteppingObject* quad;
  void create_scene_objects();

public:
	Scene();
	~Scene();
  
  // textures
  GLuint stepmapTexID;
  GLuint texmapTexID;

  // rendering
  void render();

  // controls
  Controls controls;
};
#endif
