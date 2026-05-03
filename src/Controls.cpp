#include <GLFW/glfw3.h>
#include <glm/geometric.hpp>

#include "Controls.hpp"
#include "Camera.hpp"

#include <iostream>

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

void Controls::next_camera_pos() {
	if(++view_idx >= views.size()) view_idx = 0;
	camera.set_view_matrix(views[view_idx]);
}

void Controls::prev_camera_pos() {
	if(view_idx-- == 0) view_idx = views.size() - 1;
	camera.set_view_matrix(views[view_idx]);
}

void Controls::measure(double current_time) {
	static double start_time = -1;
	static int frame_count = 0;
	static int conemap_idx = -1;

	if(!measuring) return;

	// Step through cone maps
	if (view_idx == views.size()) {
		if (static_cast<size_t>(++conemap_idx) < conemap_resources.size()) {
			selected_id = conemap_resources[conemap_idx].id;
			std::cout << conemap_resources[conemap_idx].name << std::endl;
			view_idx = 0;
			camera.set_view_matrix(views[view_idx]);
			start_time = current_time;
			frame_count = 0;
			return;
		} else {
			start_time = -1;
			frame_count = 0;
			conemap_idx = -1;
			measuring = false;
			std::cout << "Measurements finished.\n" << std::endl;
			return;
		}
	}

	// Measure
	double elapsed_time = current_time - start_time;
	if(elapsed_time < 0.5) return;
	if(elapsed_time < 1.5) {
		frame_count++;
		return;
	}
	// elapsed_time >= 1.5
	std::cout << view_idx << std::endl;
	std::cout << frame_count << std::endl;
	std::cout << 1.0 / frame_count << std::endl;

	// Step through views
	if (++view_idx < views.size()) {
		camera.set_view_matrix(views[view_idx]);
		start_time = current_time;
		frame_count = 0;
	}
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
		case GLFW_KEY_N:
			if (action == GLFW_PRESS) {
				next_camera_pos();
			}
			break;
		case GLFW_KEY_P:
			if (action == GLFW_PRESS) {
				prev_camera_pos();
			}
			break;
		case GLFW_KEY_V:
			if (action == GLFW_PRESS) {
				std::cout << "view:\n";
				glm::mat4 view = camera.get_view_matrix();
				for(int i = 0; i < 4; i++){
					for(int j = 0; j < 4; j++){
						std::cout << view[i][j] << ", ";
					}
				}
				std::cout << std::endl << std::endl;
				std::cout << "projection:\n";
				glm::mat4 proj = camera.get_projection_matrix();
				for(int i = 0; i < 4; i++){
					for(int j = 0; j < 4; j++){
						std::cout << proj[i][j] << ", ";
					}
				}
				std::cout << std::endl << std::endl;
			}
			break;
		case GLFW_KEY_M:
			if (action == GLFW_PRESS) {
				if (!measuring) {
					measuring = true;
					std::cout << "Starting measurements.\n" << std::endl;
				} else {
					measuring = false;
					std::cout << "Measurements stopped.\n" << std::endl;
				}
			}
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

