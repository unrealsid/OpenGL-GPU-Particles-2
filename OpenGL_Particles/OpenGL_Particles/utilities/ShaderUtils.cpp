#include "ShaderUtils.h"
#include <fstream>
#include <sstream>
#include <iostream>

namespace ShaderUtils
{
    GLuint loadShader(const std::string& vertexPath, const std::string& fragmentPath)
    {
        // Read vertex shader code
        std::string vertexCode;
        std::ifstream vShaderFile(vertexPath);
        std::stringstream vShaderStream;
        vShaderStream << vShaderFile.rdbuf();
        vertexCode = vShaderStream.str();

        // Read fragment shader code
        std::string fragmentCode;
        std::ifstream fShaderFile(fragmentPath);
        std::stringstream fShaderStream;
        fShaderStream << fShaderFile.rdbuf();
        fragmentCode = fShaderStream.str();

        // Compile shaders
        const char* vShaderCode = vertexCode.c_str();
        const char* fShaderCode = fragmentCode.c_str();

        GLuint vertex, fragment;
        int success;
        char infoLog[512];

        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(vertex, 512, NULL, infoLog);
            std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        }

        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(fragment, 512, NULL, infoLog);
            std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
        }

        GLuint program = glCreateProgram();
        glAttachShader(program, vertex);
        glAttachShader(program, fragment);
        glLinkProgram(program);
        glGetProgramiv(program, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(program, 512, NULL, infoLog);
            std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        }

        glDeleteShader(vertex);
        glDeleteShader(fragment);

        return program;
    }

    GLuint loadComputeShader(const std::string& computePath)
    {
        std::string computeCode;
        std::ifstream cShaderFile(computePath);
        std::stringstream cShaderStream;
        cShaderStream << cShaderFile.rdbuf();
        computeCode = cShaderStream.str();

        const char* cShaderCode = computeCode.c_str();

        GLuint compute;
        int success;
        char infoLog[512];

        compute = glCreateShader(GL_COMPUTE_SHADER);
        glShaderSource(compute, 1, &cShaderCode, NULL);
        glCompileShader(compute);
        glGetShaderiv(compute, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(compute, 512, NULL, infoLog);
            std::cerr << "ERROR::SHADER::COMPUTE::COMPILATION_FAILED\n" << infoLog << std::endl;
        }

        GLuint program = glCreateProgram();
        glAttachShader(program, compute);
        glLinkProgram(program);
        glGetProgramiv(program, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(program, 512, NULL, infoLog);
            std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        }

        glDeleteShader(compute);

        return program;
    }

    void setUniformMat4(GLuint program, const std::string& name, const glm::mat4& matrix)
    {
        GLuint location = glGetUniformLocation(program, name.c_str());
        if (location != -1)
        {
            glUniformMatrix4fv(location, 1, GL_FALSE, &matrix[0][0]);
        }
    }

    void setUniformIVec2(GLuint program, const std::string& name, const glm::ivec2& vector)
    {
        GLuint location = glGetUniformLocation(program, name.c_str());
        if (location != -1)
        {
            glUniform2i(location, vector.x, vector.y);
        }
    }

    void setUniformVec3(GLuint program, const std::string& name, const glm::vec3& vector)
    {
        GLuint location = glGetUniformLocation(program, name.c_str());
        if (location != -1)
        {
            glUniform3fv(location, 1, &vector[0]);
        }
    }

    void setUniformFloat(GLuint program, const std::string& name, float value)
    {
        GLuint location = glGetUniformLocation(program, name.c_str());
        if (location != -1)
        {
            glUniform1f(location, value);
        }
    }

    void setUniformInt(GLuint program, const std::string& name, int value)
    {
        GLuint location = glGetUniformLocation(program, name.c_str());
        if (location != -1)
        {
            glUniform1i(location, value);
        }
    }
}
