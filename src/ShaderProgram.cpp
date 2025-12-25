#include "ShaderProgram.h"
#include <iostream>

ShaderProgram::~ShaderProgram() {
    reset();
}

ShaderProgram::ShaderProgram(ShaderProgram&& other) noexcept {
    program_ = other.program_;
    other.program_ = 0;
}

ShaderProgram& ShaderProgram::operator=(ShaderProgram&& other) noexcept {
    if (this != &other) {
        reset();
        program_ = other.program_;
        other.program_ = 0;
    }
    return *this;
}

void ShaderProgram::reset() {
    if (program_) {
        glDeleteProgram(program_);
        program_ = 0;
    }
}

bool ShaderProgram::createFromVertexFragment(const char* vertexSrc, const char* fragmentSrc) {
    reset();
    GLuint vs = compile(GL_VERTEX_SHADER, vertexSrc, "Vertex");
    if (!vs) return false;
    GLuint fs = compile(GL_FRAGMENT_SHADER, fragmentSrc, "Fragment");
    if (!fs) { glDeleteShader(vs); return false; }

    program_ = glCreateProgram();
    glAttachShader(program_, vs);
    glAttachShader(program_, fs);
    bool ok = link(program_, "Graphics Program");
    glDeleteShader(vs);
    glDeleteShader(fs);
    if (!ok) { reset(); }
    return ok;
}

bool ShaderProgram::createFromCompute(const char* computeSrc) {
    reset();
    GLuint cs = compile(GL_COMPUTE_SHADER, computeSrc, "Compute");
    if (!cs) return false;
    program_ = glCreateProgram();
    glAttachShader(program_, cs);
    bool ok = link(program_, "Compute Program");
    glDeleteShader(cs);
    if (!ok) { reset(); }
    return ok;
}

GLuint ShaderProgram::compile(GLenum type, const char* source, const char* label) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);
    GLint success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[1024];
        glGetShaderInfoLog(shader, sizeof(infoLog), nullptr, infoLog);
        std::cerr << "[ShaderProgram] " << label << " shader compilation failed\n" << infoLog << std::endl;
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

bool ShaderProgram::link(GLuint program, const char* label) {
    glLinkProgram(program);
    GLint success = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[1024];
        glGetProgramInfoLog(program, sizeof(infoLog), nullptr, infoLog);
        std::cerr << "[ShaderProgram] " << label << " linking failed\n" << infoLog << std::endl;
        return false;
    }
    return true;
}
