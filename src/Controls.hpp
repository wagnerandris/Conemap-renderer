#ifndef CONTROLS_HPP
#define CONTROLS_HPP

#include "Camera.hpp"

// TODO noexcept?
class Controls {
	Camera& camera;

public:
	Controls(Camera& _camera) : camera(_camera) {}
	~Controls() {}

	void set_aspect(float aspect) { camera.set_aspect(aspect); }

	void keyboard_action(int key, int scancode, int action, int mods);
	void mouse_button_action(int button, int action, int mods);
	void mouse_move_action(double xpos, double ypos);
	void mouse_scroll_action(double xoffset, double yoffset);
};

#endif
