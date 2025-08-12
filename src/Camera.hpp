#ifndef CAMERA_HPP
#define CAMERA_HPP

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

class Camera {
public:
	Camera(glm::vec3 _eye, glm::vec3 _center, glm::vec3 _up, float _aspect);

	// getters
	inline glm::mat4 get_view_matrix() const { return view_matrix; }
	inline glm::mat4 get_projection_matrix() const { return projection_matrix; }
	inline glm::mat4 get_view_projection_matrix() const { return projection_matrix * view_matrix; }

	// setters
	// view
	void set_eye(const glm::vec3 _eye);
	void set_center(const glm::vec3 _center);
	void set_up(const glm::vec3 _up);

	// projection
	void set_fovy(const float _fovy);
	void set_aspect(const float _aspect);
	void set_zNear(const float _zNear);
	void set_zFar(const float _zFar);

	// movements
	void move(const glm::vec3 diff);
	void rotate_eye();
	void rotate_center();
	void zoom();

private:
	// view
	glm::vec3	eye;
	glm::vec3	center;
	glm::vec3	up;

	// projection
	float fovy = glm::half_pi<float>();
	float aspect;
	float zNear = 0.01f;
	float zFar = 1000.0f;

	glm::mat4	view_matrix;
	glm::mat4 projection_matrix;
};

#endif
