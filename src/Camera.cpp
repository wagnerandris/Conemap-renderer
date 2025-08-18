#include "Camera.hpp"

// GLM
#include <glm/common.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/quaternion_geometric.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>

Camera::Camera(glm::vec3 const& _eye, glm::vec3 const& _center, glm::vec3 const& _up, float _aspect)
    : eye(_eye), center(_center), up(_up), aspect(_aspect) {
  view_matrix = glm::lookAt(eye, center, up);
  projection_matrix = glm::perspective(fovy, aspect, zNear, zFar);
}

void Camera::set_eye(glm::vec3 const& _eye) {
  eye = _eye;
  view_matrix = glm::lookAt(eye, center, up);
}

void Camera::set_center(glm::vec3 const& _center) {
	center = _center;
  view_matrix = glm::lookAt(eye, center, up);
}

void Camera::set_up(glm::vec3 const& _up) {
	up = _up;
  view_matrix = glm::lookAt(eye, center, up);
}

void Camera::set_fovy(float _fovy) {
	fovy = _fovy;
  projection_matrix = glm::perspective(fovy, aspect, zNear, zFar);
}

void Camera::set_aspect(float _aspect) {
	aspect = _aspect;
  projection_matrix = glm::perspective(fovy, aspect, zNear, zFar);
}

void Camera::set_zNear(float _zNear) {
	zNear = _zNear;
  projection_matrix = glm::perspective(fovy, aspect, zNear, zFar);
}

void Camera::set_zFar(float _zFar) {
	zFar = _zFar;
  projection_matrix = glm::perspective(fovy, aspect, zNear, zFar);
}

void Camera::move(glm::vec3 const& diff) {
	eye += diff;
	center += diff;
  view_matrix = glm::lookAt(eye, center, up);
}

// TODO gimbal lock
void Camera::rotate_eye(float phi, float theta) {
	glm::vec3 center_to_eye = eye - center;

	glm::vec3 new_center_to_eye =
	// horizontal: rotate center to eye around up
	glm::rotate(phi, up) *
	// vertical: rotate center to eye around right (up x center to eye)
	glm::rotate(-theta, glm::normalize(glm::cross(up, center_to_eye))) *
	glm::vec4(center_to_eye, 0.0f);

	glm::vec3 new_eye = center + new_center_to_eye;
	set_eye(new_eye);
}

void Camera::rotate_center(float phi, float theta) {
	glm::vec3 eye_to_center = center - eye;

	glm::vec3 new_eye_to_center =
	// horizontal: rotate eye to center around up
	glm::rotate(phi, up) *
	// vertical: rotate eye to center around right (eye to center x up)
	glm::rotate(theta, glm::normalize(glm::cross(eye_to_center, up))) *
	glm::vec4(eye_to_center, 0.0f);

	glm::vec3 new_center = eye + new_eye_to_center;
	set_center(new_center);
}

void Camera::zoom(double yoffset) {
	glm::vec3 eye_to_center = center - eye;
	set_eye(eye + eye_to_center * static_cast<float>(yoffset));
	// change fovy or move towards center or both?
}
