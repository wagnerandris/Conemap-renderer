#ifndef FILE_UTILS_H
#define FILE_UTILS_H

// STD
#include <filesystem>
#include <string>
#include <vector>

// GL
#include <GL/gl.h>


struct TextureResource {
	std::filesystem::path path;
	std::string name;
	GLuint id = 0;
};

std::string read_text_file(const std::filesystem::path &path);
void load_shader_from_file(const std::filesystem::path &path, const GLuint shader);
GLuint load_texture_from_file(const std::filesystem::path &path, bool conemap = false);
std::string load_textures(const std::vector<std::filesystem::path> &paths, std::vector<TextureResource> &resources, bool conemap = false);

#endif
