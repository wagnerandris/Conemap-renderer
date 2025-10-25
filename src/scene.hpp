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

	// rendering
	void render();
	
	// settings
	float depth = 0.25f;
	int steps = 8;
	int display_mode = 1;
	bool show_convergence = true;

	// controls
	Controls controls;
};
#endif
