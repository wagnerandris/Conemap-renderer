#include "Camera.hpp"

// GLM
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

Camera::Camera(glm::vec3 _eye, glm::vec3 _center, glm::vec3 _up, float _aspect)
    : eye(_eye), center(_center), up(_up), aspect(_aspect) {
  view_matrix = glm::lookAt(eye, center, up);
  projection_matrix = glm::perspective(fovy, aspect, zNear, zFar);
}

void Camera::set_eye(const glm::vec3 _eye) {
  eye = _eye;
  view_matrix = glm::lookAt(eye, center, up);
}

void Camera::set_center(const glm::vec3 _center) {
	center = _center;
  view_matrix = glm::lookAt(eye, center, up);
}

void Camera::set_up(const glm::vec3 _up) {
	up = _up;
  view_matrix = glm::lookAt(eye, center, up);
}

void Camera::set_fovy(const float _fovy) {
	fovy = _fovy;
  projection_matrix = glm::perspective(fovy, aspect, zNear, zFar);
}

void Camera::set_aspect(const float _aspect) {
	aspect = _aspect;
  projection_matrix = glm::perspective(fovy, aspect, zNear, zFar);
}

void Camera::set_zNear(const float _zNear) {
	zNear = _zNear;
  projection_matrix = glm::perspective(fovy, aspect, zNear, zFar);
}

void Camera::set_zFar(const float _zFar) {
	zFar = _zFar;
  projection_matrix = glm::perspective(fovy, aspect, zNear, zFar);
}

void Camera::move(const glm::vec3 diff) {
	eye += diff;
	center += diff;
  view_matrix = glm::lookAt(eye, center, up);
}

// TODO
void Camera::rotate_eye() {
	// horizontal: rotate center to eye around up
	// vertical: rotate center to eye around center to eye x up
}

void Camera::rotate_center() {
	// horizontal: rotate eye to center around up
	// vertical: rotate eye to center around eye to center x up
}

void Camera::zoom() {
	// change fovy or move towards center or both?
}
