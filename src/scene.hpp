#ifndef SCENE_HPP
#define SCENE_HPP

// Utils
#include "ConeSteppingObject.hpp"
#include "Camera.hpp"
#include "Controls.hpp"

class Scene {
	// program
	GLuint program;

	// camera
	Camera camera;

public:
	Scene();
	~Scene();
	
	// camera controls
	Controls controls;
	
	// objects
	ConeSteppingObject quad;

	// rendering settings
	int steps = 8;
	int display_mode = 1;
	bool show_convergence = true;

	// rendering
	void render();
};
#endif
