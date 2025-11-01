#ifndef CONE_STEPPING_OBJECT_HPP
#define CONE_STEPPING_OBJECT_HPP

// GLFW
#include <GLFW/glfw3.h>

// GLM
#include <glm/glm.hpp>

// STD
#include <vector>

// vertex structure
struct PosUVVertex {
	glm::vec3 pos;
	glm::vec2 uv;
};

// object structure
struct ConeSteppingObject {
	GLuint vao;
	GLuint vbo;
	GLuint stepmapTex;
	GLuint texmapTex;
	float depth = 0.25f;

	ConeSteppingObject(const std::vector<PosUVVertex> vertices);
	~ConeSteppingObject();
};

#endif
