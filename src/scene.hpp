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

	// objects
	ConeSteppingObject* quad;
	void create_scene_objects();

public:
	Scene();
	~Scene();
	
	// textures
	GLuint stepmapTexID;
	GLuint texmapTexID;

	float depth = 0.25f;
	
	// rendering
	void render();
	
	int display_mode = 1;

	// controls
	Controls controls;
};
#endif
