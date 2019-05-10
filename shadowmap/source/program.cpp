#include "program.h"
#include "glm/gtc/type_ptr.hpp"
#include <fstream>

bool OpenGLProgram::setUniform(std::string name, glm::mat4 value) const
{
    if (!glIsProgram(program))
        return false;
    glUseProgram(program);
    auto loc = glGetUniformLocation(program, name.c_str());
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(value));
    return true;
}

bool OpenGLProgram::setUniform(std::string name, glm::vec3 value) const
{
    if (!glIsProgram(program))
        return false;
    glUseProgram(program);
    auto loc = glGetUniformLocation(program, name.c_str());
    glUniform3f(loc, value.x, value.y, value.z);
    return true;
}

bool OpenGLProgram::setUniform(std::string name, int value) const
{
    if (!glIsProgram(program))
        return false;
    glUseProgram(program);
    auto loc = glGetUniformLocation(program, name.c_str());
    glUniform1i(loc, value);
    return true;
}

bool OpenGLProgram::appendShader(const std::string &shader_content, GLenum shader_type) 
{
    if (program == 0)
        program = glCreateProgram();
    auto shader = glCreateShader(shader_type);
    auto ptr = shader_content.c_str();
    glShaderSource(shader, 1, &ptr, nullptr);
    glCompileShader(shader);
    // Check if the shader is correctly compiled
    static char log[500];
    glGetShaderInfoLog(shader, 500, NULL, log);
    if (log[0]) {
        printf("%s\n", log);
        return false;
    }
    glAttachShader(program, shader);
    return true;
}

bool OpenGLProgram::appendShaderFromFile(const std::string &path, GLenum shader_type) 
{
    std::ifstream input(path); 
    std::string content, line;
    while (std::getline(input, line))
        content += line + "\n";
    return appendShader(content, shader_type);
}

OpenGLProgram::operator int()
{
    return program;
}