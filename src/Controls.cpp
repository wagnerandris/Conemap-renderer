#include "Controls.hpp"
#include "src/Camera.hpp"
#include <GLFW/glfw3.h>

void Controls::keyboard_action(int key, int scancode, int action, int mods) {}
// move focus and camera together

void Controls::mouse_button_action(int button, int action, int mods) {}
// middle click: move around focus
// right click: move focus around

void Controls::mouse_move_action(double xdiff, double ydiff) {
	camera.rotate_eye(xdiff / 32.0, ydiff / 32.0);
	// TODO separate speeds for mouse and touchpad/joystick
}

void Controls::mouse_scroll_action(double xoffset, double yoffset) {}
// closer/away from focus

