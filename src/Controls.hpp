#ifndef CONTROLS_HPP
#define CONTROLS_HPP

#include "Camera.hpp"
#include "file_utils.hpp"
#include <vector>

class Controls {
	Camera& camera;

	// mouse
	bool middle_down = false;
	bool right_down = false;

	// keys
	float w_down = 0;
	float a_down = 0;
	float s_down = 0;
	float d_down = 0;
	float q_down = 0;
	float e_down = 0;
	
	float speed = 0.5;

	const std::vector<glm::mat4> views = {
		{-1, 0, -0, 0,
			0, 0, 1, 0,
			0, 1, -0, 0,
			-0, -0, -1, 1},
			// -0, -0, -1.41421, 1},
		{-1, -0, 0, 0,
			0, 0.2588190451, 0.9659258263, 0,
			-0, 0.9659258263, -0.2588190451, 0,
			-0, -0, -0.9659258263, 1},
			// -0, -0, -1.41421, 1},
		{-1, -0, 0, 0,
			0, 0.5, 0.8660254038, 0,
			-0, 0.8660254038, -0.5, 0,
			-0, -0, -0.8660254038, 1},
			// -0, -0, -1.41421, 1},
		{-1, 0, -0, 0,
			0, 0.707107, 0.707107, 0,
			0, 0.707107, -0.707107, 0,
			-0, -0, -0.707107, 1},
			// -0, -0, -1.41421, 1},
		{-1, -0, 0, 0,
			0, 0.8660254038, 0.5, 0,
			-0, 0.5, -0.8660254038, 0,
			-0, -0, -0.5, 1},
			// -0, -0, -1.41421, 1},
		{-1, -0, 0, 0,
			0, 0.9659258263, 0.2588190451, 0,
			-0, 0.2588190451, -0.9659258263, 0,
			-0, -0, -0.2588190451, 1}
			// -0, -0, -1.41421, 1},
	};
	size_t view_idx = views.size();

	bool measuring = false;

	void next_camera_pos();
	void prev_camera_pos();

	std::vector<TextureResource> &conemap_resources;
	GLuint &selected_id;

public:
	Controls(Camera& _camera, std::vector<TextureResource> &_conemap_resources, GLuint &_selected_id) : camera(_camera), conemap_resources(_conemap_resources), selected_id(_selected_id) {}
	~Controls() {}

	void move(double delta_time);
	void measure(double current_time);
	void set_aspect(float aspect) { camera.set_aspect(aspect); }

	void keyboard_action(int key, int scancode, int action, int mods);
	void mouse_button_action(int button, int action, int mods);
	void mouse_move_action(double xpos, double ypos);
	void mouse_scroll_action(double xoffset, double yoffset);
};

#endif
