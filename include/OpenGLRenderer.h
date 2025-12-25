#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>

/// Simple OpenGL renderer for testing ECS with 2D quads
class OpenGLRenderer {
public:
    OpenGLRenderer();
    ~OpenGLRenderer();

    // Initialize GLFW and OpenGL context
    bool initialize(int width, int height, const std::string& title);
    
    // Cleanup
    void shutdown();
    
    // Check if window should close
    bool shouldClose() const;
    
    // Begin frame (clear screen)
    void beginFrame();
    
    // End frame (swap buffers)
    void endFrame();
    
    // Draw a 2D quad with transform matrix
    void drawQuad(const glm::mat4& transform, const glm::vec3& color);
    
    // Get window
    GLFWwindow* getWindow() const { return window; }

private:
    GLFWwindow* window = nullptr;
    GLuint shaderProgram = 0;
    GLuint VAO = 0;
    GLuint VBO = 0;
    GLuint EBO = 0;
    
    // Shader compilation helper
    GLuint compileShader(const char* source, GLenum type);
    GLuint createShaderProgram(const char* vertexSrc, const char* fragmentSrc);
    
    // Create quad geometry
    void createQuadGeometry();
};
