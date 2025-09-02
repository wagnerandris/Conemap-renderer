// GLEW
#include <GL/glew.h>

#include "ConeSteppingObject.hpp"

ConeSteppingObject::ConeSteppingObject(
    const std::vector<ConeSteppingVertex> vertices,
    const GLuint& stepmapTex_, const GLuint& texmapTex_)
    : stepmapTex(stepmapTex_), texmapTex(texmapTex_) {

  // create VAO
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  // create VBO
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(ConeSteppingVertex), vertices.data(),
               GL_STATIC_DRAW);

  // setup VAO
  glEnableVertexAttribArray(0); // position
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ConeSteppingVertex),
                        (void *)offsetof(ConeSteppingVertex, pos));

  glEnableVertexAttribArray(1); // normal
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(ConeSteppingVertex),
                        (void *)offsetof(ConeSteppingVertex, normal));

  glEnableVertexAttribArray(2); // tangent
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(ConeSteppingVertex),
                        (void *)offsetof(ConeSteppingVertex, tangent));

  glEnableVertexAttribArray(3); // binormal
  glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(ConeSteppingVertex),
                        (void *)offsetof(ConeSteppingVertex, binormal));

  glEnableVertexAttribArray(4); // uv
  glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, sizeof(ConeSteppingVertex),
                        (void *)offsetof(ConeSteppingVertex, uv));

  // unbind
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

ConeSteppingObject::~ConeSteppingObject() {
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
}

