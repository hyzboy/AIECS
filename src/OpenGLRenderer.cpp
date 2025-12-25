#include "OpenGLRenderer.h"
#include <iostream>
#include <cstring>

// Vertex shader for 2D quad rendering with transform
const char* vertexShaderSource = R"(
#version 460 core
layout (location = 0) in vec2 aPos;

uniform mat4 uTransform;
uniform mat4 uProjection;

void main() {
    gl_Position = uProjection * uTransform * vec4(aPos, 0.0, 1.0);
}
)";

// Fragment shader with uniform color
const char* fragmentShaderSource = R"(
#version 460 core
out vec4 FragColor;

uniform vec3 uColor;

void main() {
    FragColor = vec4(uColor, 1.0);
}
)";

OpenGLRenderer::OpenGLRenderer() {
}

OpenGLRenderer::~OpenGLRenderer() {
    shutdown();
}

bool OpenGLRenderer::initialize(int width, int height, const std::string& title) {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return false;
    }
    
    // Set OpenGL version to 4.6 Core
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    // Create window
    window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }
    
    glfwMakeContextCurrent(window);
    
    // Initialize GLEW
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW: " << glewGetErrorString(err) << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return false;
    }
    
    // Print OpenGL version
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    
    // Create shader program
    shaderProgram = createShaderProgram(vertexShaderSource, fragmentShaderSource);
    if (shaderProgram == 0) {
        return false;
    }
    
    // Create quad geometry
    createQuadGeometry();
    
    // Set viewport
    glViewport(0, 0, width, height);
    
    // Enable blending for transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    return true;
}

void OpenGLRenderer::shutdown() {
    if (VAO) glDeleteVertexArrays(1, &VAO);
    if (VBO) glDeleteBuffers(1, &VBO);
    if (EBO) glDeleteBuffers(1, &EBO);
    if (shaderProgram) glDeleteProgram(shaderProgram);
    
    if (window) {
        glfwDestroyWindow(window);
        window = nullptr;
    }
    glfwTerminate();
}

bool OpenGLRenderer::shouldClose() const {
    return window && glfwWindowShouldClose(window);
}

void OpenGLRenderer::beginFrame() {
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void OpenGLRenderer::endFrame() {
    glfwSwapBuffers(window);
    glfwPollEvents();
}

void OpenGLRenderer::drawQuad(const glm::mat4& transform, const glm::vec3& color) {
    glUseProgram(shaderProgram);
    
    // Set uniforms
    GLint transformLoc = glGetUniformLocation(shaderProgram, "uTransform");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, &transform[0][0]);
    
    // Set orthographic projection (2D, -10 to 10 in X and Y)
    glm::mat4 projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, -1.0f, 1.0f);
    GLint projectionLoc = glGetUniformLocation(shaderProgram, "uProjection");
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, &projection[0][0]);
    
    GLint colorLoc = glGetUniformLocation(shaderProgram, "uColor");
    glUniform3fv(colorLoc, 1, &color[0]);
    
    // Draw quad
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

GLuint OpenGLRenderer::compileShader(const char* source, GLenum type) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);
    
    // Check compilation errors
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "Shader compilation failed: " << infoLog << std::endl;
        return 0;
    }
    
    return shader;
}

GLuint OpenGLRenderer::createShaderProgram(const char* vertexSrc, const char* fragmentSrc) {
    GLuint vertexShader = compileShader(vertexSrc, GL_VERTEX_SHADER);
    if (vertexShader == 0) return 0;
    
    GLuint fragmentShader = compileShader(fragmentSrc, GL_FRAGMENT_SHADER);
    if (fragmentShader == 0) {
        glDeleteShader(vertexShader);
        return 0;
    }
    
    // Link shaders
    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    
    // Check linking errors
    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        std::cerr << "Shader program linking failed: " << infoLog << std::endl;
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return 0;
    }
    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    return program;
}

void OpenGLRenderer::createQuadGeometry() {
    // Quad vertices (unit quad centered at origin)
    float vertices[] = {
        // positions
        -0.5f, -0.5f,  // bottom-left
         0.5f, -0.5f,  // bottom-right
         0.5f,  0.5f,  // top-right
        -0.5f,  0.5f   // top-left
    };
    
    // Indices for two triangles
    unsigned int indices[] = {
        0, 1, 2,  // first triangle
        2, 3, 0   // second triangle
    };
    
    // Create VAO, VBO, EBO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    // Position attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glBindVertexArray(0);
}
