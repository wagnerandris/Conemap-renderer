// STD
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

// GLEW
#include <GL/glew.h>

// stb
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include "file_utils.hpp"

std::string read_text_file(const std::filesystem::path &path) {
  // open file and seek to the end
  std::ifstream file(path, std::ios::in | std::ios::binary | std::ios::ate);
  if (!file.is_open())
    fprintf(stderr, "Error: Could not open %s\n", path.c_str());

  std::ifstream::pos_type bytesize = file.tellg();
  if (bytesize == std::ifstream::pos_type(-1))
    fprintf(stderr, "Error: Could not read %s\n", path.c_str());

  file.seekg(0, std::ios::beg); // reset cursor to beginning

  std::vector<char> bytes(bytesize);
  file.read(bytes.data(), bytesize);

  return std::string(bytes.data(), bytesize);
}

void load_shader_from_file(const GLuint shader,
                                  const std::filesystem::path &path) {
  const std::string shader_string = read_text_file(path);
  const char *shader_text = shader_string.data();
  const GLint shader_length = shader_string.length();
  glShaderSource(shader, 1, &shader_text, &shader_length);
  glCompileShader(shader);
}

GLuint load_texture_from_file(const char *filepath) {
  stbi_set_flip_vertically_on_load(true); // OpenGL expects 0.0 at bottom

  int width, height, channels;
  unsigned char *data = stbi_load(filepath, &width, &height, &channels, 4);
  if (!data) {
    std::fprintf(stderr, "Could not load texture from %s.\n", filepath);
    return 0;
  }
  
  GLuint textureID;
  glCreateTextures(GL_TEXTURE_2D, 1, &textureID);

  glTextureStorage2D(textureID, 1, GL_RGBA8, width, height);
  glTextureSubImage2D(textureID, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);

  glTextureParameteri(textureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTextureParameteri(textureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTextureParameteri(textureID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTextureParameteri(textureID, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTextureParameteri(textureID, GL_TEXTURE_WRAP_T, GL_REPEAT);

  stbi_image_free(data);
  return textureID;
}
