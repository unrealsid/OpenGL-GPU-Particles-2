#pragma once
#include <string>
#include <glm.hpp>
#include "../glad/glad.h"

namespace ShaderUtils
{
    GLuint loadShader(const std::string& vertexPath, const std::string& fragmentPath);
    GLuint loadComputeShader(const std::string& computePath);
    void setUniformMat4(GLuint program, const std::string& name, const glm::mat4& matrix);
    void setUniformIVec2(GLuint program, const std::string& name, const glm::ivec2& vector);
    void setUniformVec3(GLuint program, const std::string& name, const glm::vec3& vector);
    void setUniformFloat(GLuint program, const std::string& name, float value);
    void setUniformInt(GLuint program, const std::string& name, int value);
} 
