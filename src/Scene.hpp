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
	Scene(std::vector<TextureResource> &conemap_resources);
	~Scene();
	
	// objects
	ConeSteppingObject quad;
	
	// camera controls
	Controls controls;

	// rendering settings
	int cone_steps = 128;
	int binary_steps = 8;
	int display_mode = 1;
	bool cell_max_trace = false;
	bool show_convergence = true;

	// rendering
	void render();
};
#endif
