// GLEW
#include <GL/glew.h>
#include <GL/gl.h>

// GLM
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>

// STD
#include <vector>
#include <iostream>

#include "ConeSteppingObject.hpp"
#include "file_utils.hpp"

#include "scene.hpp"

void Scene::create_shaders() {
	// After shader compile
	GLint success;
	GLchar infoLog[512];

	// stages
	const GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	load_shader_from_file(vertex_shader, "ConeStepMapping.vert");
	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
	if (!success) {
			glGetShaderInfoLog(vertex_shader, 512, NULL, infoLog);
			std::cerr << "Shader Compilation Error: " << infoLog << std::endl;
	}

	const GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	load_shader_from_file(fragment_shader, "RelaxedConeStepMapping.frag");

	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
	if (!success) {
			glGetShaderInfoLog(fragment_shader, 512, NULL, infoLog);
			std::cerr << "Shader Compilation Error: " << infoLog << std::endl;
	}

	// program
	program = glCreateProgram();
	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);
	glLinkProgram(program);

	// After program link
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success) {
			glGetProgramInfoLog(program, 512, NULL, infoLog);
			std::cerr << "Program Linking Error: " << infoLog << std::endl;
	}
}

void Scene::create_scene_objects() {
	// quad
	// setup vertices
	const std::vector<PosUVVertex> vertices = {
			// position						 UV
			{{-1.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
			{{1.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
			{{1.0f, 0.0f, -1.0f}, {1.0f, 1.0f}},

			{{1.0f, 0.0f, -1.0f}, {1.0f, 1.0f}},
			{{-1.0f, 0.0f, -1.0f}, {0.0f, 1.0f}},
			{{-1.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
	};

	quad = new ConeSteppingObject(vertices, stepmapTexID, texmapTexID);
}

Scene::Scene() : camera(Camera(glm::vec3(0.0f, 1.0f, -1.0f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 16.0f/9.0f)), controls(Controls(camera)) {
	create_shaders();
	create_scene_objects();
}

Scene::~Scene() {
	// geometry
	delete quad;

	// shaders
	glDeleteProgram(program);
}

void Scene::render() {
	glEnable(GL_CULL_FACE);
	glClearColor(0.1f, 0.2f, 0.6f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// set time dependent stuff
	glm::mat4 worldMatrix = glm::identity<glm::mat4>();
	
	// set program
	glUseProgram(program);

	// get uniform locations and set uniform values
	// vertex
	GLuint worldViewMatrixLoc = glGetUniformLocation(program, "worldViewMatrix");
	GLuint projectionMatrixLoc = glGetUniformLocation(program, "projectionMatrix");

	glUniformMatrix4fv(worldViewMatrixLoc, 1, false,
										 glm::value_ptr(camera.get_view_matrix() * worldMatrix));
	glUniformMatrix4fv(projectionMatrixLoc, 1, false,
										 glm::value_ptr(camera.get_projection_matrix()));

	// fragment
	GLuint depthLoc = glGetUniformLocation(program, "depth");
	GLuint stepsLoc = glGetUniformLocation(program, "steps");
	GLuint display_modeLoc = glGetUniformLocation(program, "display_mode");
	GLboolean show_convergenceLoc = glGetUniformLocation(program, "show_convergence");
	GLuint stepmapLoc = glGetUniformLocation(program, "stepmap");
	GLuint texmapLoc = glGetUniformLocation(program, "texmap");

	// set uniform values
	glUniform1f(depthLoc, depth);
	glUniform1i(stepsLoc, steps);
	glUniform1i(display_modeLoc, display_mode);
	glUniform1i(show_convergenceLoc, show_convergence);

	// quad
	// bind textures
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, quad->stepmapTex);
	glUniform1i(stepmapLoc, 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, quad->texmapTex);
	glUniform1i(texmapLoc, 1);

	// bind vertex array
	glBindVertexArray(quad->vao);

	// draw
	glDrawArrays(GL_TRIANGLES, 0, 6);

	// unbind
	glBindVertexArray(0);
	glUseProgram(0);
}
