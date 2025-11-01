// GLEW
#include <GL/glew.h>
#include <glm/geometric.hpp>

#include "ConeSteppingObject.hpp"

ConeSteppingObject::ConeSteppingObject(const std::vector<PosUVVertex> vertices) {
	// vertex structure
	struct ConeSteppingVertex {
		glm::vec3 pos;
		glm::vec3 normal;
		glm::vec3 tangent;
		glm::vec3 bitangent;
		glm::vec2 uv;
	};

	// vertices to be sent to GPU
	std::vector<ConeSteppingVertex> csvs;

	// find the tangent and bitangent and normal for each triangle face
	for (size_t i = 0; i < vertices.size(); i += 3) {
		glm::vec3 dp1 = vertices[i + 1].pos - vertices[i].pos;
		glm::vec3 dp2 = vertices[i + 2].pos - vertices[i].pos;
		glm::vec2 duv1 = vertices[i + 1].uv - vertices[i].uv;
		glm::vec2 duv2 = vertices[i + 2].uv - vertices[i].uv;

		// duv1 and duv2 form a base in the uv plane
		// we are looking for the inverse, which transforms these vectors to u and v

		// we multiply the corresponding dp1 and dp2 with the inverse to get the
		// 3D object space direction vectors corresponding to u and v
		
		float det = 1.0f / (duv1.x * duv2.y - duv1.y * duv2.x);

		glm::vec3 tangent = det * (dp1 * duv2.y - dp2 * duv1.y);
		glm::vec3 bitangent = - det * (dp1 * duv1.x - dp2 * duv2.x); // z faces the other way
		
		// the normal is ortogonal to both tangent and bitangent
		glm::vec3 normal = glm::cross(tangent, bitangent);

		csvs.push_back(ConeSteppingVertex{vertices[i].pos, normal, tangent, bitangent, vertices[i].uv});
		csvs.push_back(ConeSteppingVertex{vertices[i + 1].pos, normal, tangent, bitangent, vertices[i + 1].uv});
		csvs.push_back(ConeSteppingVertex{vertices[i + 2].pos, normal, tangent, bitangent, vertices[i + 2].uv});
	}

	// create VAO
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// create VBO
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, csvs.size() * sizeof(ConeSteppingVertex), csvs.data(),
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
												(void *)offsetof(ConeSteppingVertex, bitangent));

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

