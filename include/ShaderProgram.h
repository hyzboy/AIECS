#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>

// Simple RAII wrapper for OpenGL shader programs
// Supports creating programs from vertex+fragment shaders and compute shaders
class ShaderProgram {
public:
    ShaderProgram() = default;
    ~ShaderProgram();

    // Non-copyable, movable
    ShaderProgram(const ShaderProgram&) = delete;
    ShaderProgram& operator=(const ShaderProgram&) = delete;
    ShaderProgram(ShaderProgram&& other) noexcept;
    ShaderProgram& operator=(ShaderProgram&& other) noexcept;

    // Build program from vertex/fragment sources
    bool createFromVertexFragment(const char* vertexSrc, const char* fragmentSrc);

    // Build program from compute source
    bool createFromCompute(const char* computeSrc);

    // Bind program for use
    void use() const { if (program_) glUseProgram(program_); }

    // Program id
    GLuint id() const { return program_; }

    // Helpers
    GLint uniformLocation(const char* name) const { return program_ ? glGetUniformLocation(program_, name) : -1; }
    void setUniformMat4(const char* name, const glm::mat4& m) const {
        GLint loc = uniformLocation(name);
        if (loc >= 0) {
            glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(m));
        }
    }

    // Reset and delete program
    void reset();

private:
    static GLuint compile(GLenum type, const char* source, const char* label);
    static bool link(GLuint program, const char* label);

    GLuint program_ = 0;
};
