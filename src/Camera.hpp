#ifndef CAMERA_HPP
#define CAMERA_HPP

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

class Camera {
public:
	Camera(glm::vec3 const& _eye, glm::vec3 const& _center, glm::vec3 const& _up, float _aspect);

	// getters
	inline glm::mat4 get_view_matrix() const { return view_matrix; }
	inline glm::mat4 get_projection_matrix() const { return projection_matrix; }
	inline glm::mat4 get_view_projection_matrix() const { return projection_matrix * view_matrix; }

	// setters
	// view
	void set_eye(glm::vec3 const& _eye);
	void set_center(glm::vec3 const& _center);
	void set_up(glm::vec3 const& _up);

	// projection
	void set_fovy(float _fovy);
	void set_aspect(float _aspect);
	void set_zNear(float _zNear);
	void set_zFar(float _zFar);

	// movements
	void move(glm::vec3 const& diff);
	void rotate_eye(float phi, float theta);
	void rotate_center(float phi, float theta);
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
