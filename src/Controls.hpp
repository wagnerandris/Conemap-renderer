#ifndef CONTROLS_HPP
#define CONTROLS_HPP

#include "Camera.hpp"

// TODO noexcept?
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

public:
	Controls(Camera& _camera) : camera(_camera) {}
	~Controls() {}

	void set_aspect(float aspect) { camera.set_aspect(aspect); }
	void move(double delta_time);

	void keyboard_action(int key, int scancode, int action, int mods);
	void mouse_button_action(int button, int action, int mods);
	void mouse_move_action(double xpos, double ypos);
	void mouse_scroll_action(double xoffset, double yoffset);
};

#endif
