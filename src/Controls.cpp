#include <GLFW/glfw3.h>
#include <glm/geometric.hpp>

#include "Controls.hpp"
#include "src/Camera.hpp"

void Controls::move(double delta_time) {
	glm::vec3 dirs = glm::vec3(d_down - a_down, e_down - q_down, w_down - s_down);
	if (glm::length(dirs) == 0) return;
	
	glm::vec3 dir = glm::normalize(dirs) * speed * static_cast<float>(delta_time);

	glm::vec3 eye_to_center = camera.get_center() - camera.get_eye();
	glm::vec3 up = camera.get_up();
	glm::vec3 right = glm::normalize(glm::cross(eye_to_center, up));

	glm::vec3 diff = dir.x * right + dir.y * up + dir.z * eye_to_center;

	camera.move(diff);
}

void Controls::keyboard_action(int key, int scancode, int action, int mods) {
	switch (key) {
		case GLFW_KEY_W:
			if (action == GLFW_PRESS) {
				w_down = 1;
			} else if (action == GLFW_RELEASE) {
				w_down = 0;
			}
			break;
		case GLFW_KEY_A:
			if (action == GLFW_PRESS) {
				a_down = 1;
			} else if (action == GLFW_RELEASE) {
				a_down = 0;
			}
			break;
		case GLFW_KEY_S:
			if (action == GLFW_PRESS) {
				s_down = 1;
			} else if (action == GLFW_RELEASE) {
				s_down = 0;
			}
			break;
		case GLFW_KEY_D:
			if (action == GLFW_PRESS) {
				d_down = 1;
			} else if (action == GLFW_RELEASE) {
				d_down = 0;
			}
			break;
		case GLFW_KEY_Q:
			if (action == GLFW_PRESS) {
				q_down = 1;
			} else if (action == GLFW_RELEASE) {
				q_down = 0;
			}
			break;
		case GLFW_KEY_E:
			if (action == GLFW_PRESS) {
				e_down = 1;
			} else if (action == GLFW_RELEASE) {
				e_down = 0;
			}
			break;
	}
}

void Controls::mouse_button_action(int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
		if (action == GLFW_PRESS) {
			middle_down = true;
		} else if (action == GLFW_RELEASE) {
			middle_down = false;
		}
	} else
	if (button == GLFW_MOUSE_BUTTON_RIGHT) {
		if (action == GLFW_PRESS) {
			right_down = true;
		} else if (action == GLFW_RELEASE) {
			right_down = false;
		}
	}
}

void Controls::mouse_move_action(double xdiff, double ydiff) {
	if (middle_down && !right_down) {
		camera.rotate_eye(xdiff / 32.0, ydiff / 32.0);
	} else
	if (!middle_down && right_down) {
		camera.rotate_center(xdiff / 32.0, ydiff / 32.0);
	}
}

void Controls::mouse_scroll_action(double xoffset, double yoffset) {
	camera.zoom(yoffset);
}
// closer/away from focus

