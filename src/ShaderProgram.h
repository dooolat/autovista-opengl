#pragma once

#include <filesystem>

#include <glad/gl.h>

GLuint createProgram(const std::filesystem::path& shaderDir);