#pragma once

#include <glad/glad.h>

#include "shader_program.hpp"

class ShaderProgramBuilder{
public:
    ShaderProgramBuilder();

    ShaderProgramBuilder& withVertexShader(std::string filePath);
    ShaderProgramBuilder& withFragmentShader(std::string filePath);
    ShaderProgramBuilder& withGeometryShader(std::string filePath);

    ShaderProgram build();
private:
    unsigned int m_programId;
    ShaderProgramBuilder& withShader(std::string filePath, std::string shaderType, unsigned int glShaderConst);
    void checkCompileErrors(GLuint shader, std::string type) const;
};
