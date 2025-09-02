#ifndef CONE_STEPPING_OBJECT_HPP
#define CONE_STEPPING_OBJECT_HPP

// GLFW
#include <GLFW/glfw3.h>

// GLM
#include <glm/glm.hpp>

// STD
#include <vector>


// vertex structure
struct ConeSteppingVertex {
  glm::vec3 pos;
  glm::vec3 normal;
  glm::vec3 tangent;
  glm::vec3 binormal;
  glm::vec2 uv;
};

// object structure
struct ConeSteppingObject {
	GLuint vao;
	GLuint vbo;
	const GLuint& stepmapTex;
	const GLuint& texmapTex;

	ConeSteppingObject(const std::vector<ConeSteppingVertex> vertices, const GLuint& stepmapTex, const GLuint& texmapTex);
	~ConeSteppingObject();
};

#endif
