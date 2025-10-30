#ifndef FILE_UTILS_H
#define FILE_UTILS_H

// STD
#include <filesystem>
#include <string>

// GLEW
#include <GL/glew.h>

std::string read_text_file(const std::filesystem::path &path);
void load_shader_from_file(const std::filesystem::path &path, const GLuint shader);
GLuint load_texture_from_file(const std::filesystem::path &path);

#endif
