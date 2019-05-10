#pragma once
#ifndef  PROGRAM_H
#define PROGRAM_H

#include "g3/include/GL/gl3w.h"
#include <string>
#include "glm/glm.hpp"


class OpenGLProgram {
public:
    OpenGLProgram():program(0){}
    template<typename ...Args>
    OpenGLProgram(bool file, Args ...args) : program(glCreateProgram()) {
        initShader(file, args...);
        glLinkProgram(program);
    };

    bool setUniform(std::string name, glm::mat4 value) const;
    bool setUniform(std::string name, glm::vec3 value) const;
    bool setUniform(std::string name, int value) const;
    bool appendShader(const std::string &shader_content, GLenum shader_type) ;
    bool appendShaderFromFile(const std::string &path, GLenum shader_type) ;
    inline bool link() const { glLinkProgram(program); return glIsProgram(program); }
    inline GLuint getProgram() const { return program; }
    inline bool isValid() const { return glIsProgram(program); }
    operator int();
private:
    template<typename ...Args>
    void initShader(bool file, const std::string &shader_content, GLenum shader_type, Args ...args)
    {
        if (file)
            appendShaderFromFile(shader_content, shader_type);
        else appendShader(shader_content, shader_type); 
        initShader(file, args...);
    }
    void initShader(bool file, const std::string &shader_content, GLenum shader_type)
    {
        if (file)
            appendShaderFromFile(shader_content, shader_type);
        else appendShader(shader_content, shader_type); 
    }
    GLuint program;
};

#endif //  
