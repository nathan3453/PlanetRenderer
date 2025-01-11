#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <GL/glew.h>
#include <assimp/Importer.hpp>
#include <assimp/Scene.h>
#include <assimp/PostProcess.h>
#include <glm/glm.hpp>

bool ReadFile(const char* pFileName, std::string& outFile);
bool WriteFile(const char* pFileName, const std::string& inFile);
void AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType);
GLuint CompileComputeShader(const char* source);
GLuint GetUniformLocation(GLuint shaderProgram, const char* uniformName);
void LoadTexture(GLuint* texture, const char* path);
void SaveTextureToFile(GLuint texture, const std::string& path, int width, int height, GLenum format);