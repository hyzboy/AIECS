#include "RenderComponent.h"
#include "TransformComponent.h"
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

RenderComponent::RenderComponent(const std::string& name)
    : Component(name) {
}

RenderComponent::~RenderComponent() {
    cleanup();
}

void RenderComponent::initializeGL() {
    if (glInitialized) return;

    // Rectangle vertices from -0.5,-0.5 to +0.5,+0.5 (two triangles)
    float vertices[] = {
        // First triangle
         0.5f,  0.5f, 0.0f,  // top right
         0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f,  0.5f, 0.0f,  // top left
        // Second triangle
         0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  // bottom left
        -0.5f,  0.5f, 0.0f   // top left
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glInitialized = true;
}

void RenderComponent::render(unsigned int shaderProgram, GameEntity* entity) {
    if (!isVisible || !glInitialized || !entity) return;

    // Get the TransformComponent
    auto transform = entity->getComponent<TransformComponent>();
    if (!transform) return;

    // Get the world matrix from TransformComponent
    glm::mat4 modelMatrix = transform->getWorldMatrix();

    // Set the model matrix uniform in shader
    GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
    if (modelLoc != -1) {
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));
    }

    // Set the color uniform in shader
    GLint colorLoc = glGetUniformLocation(shaderProgram, "rectColor");
    if (colorLoc != -1) {
        glUniform4fv(colorLoc, 1, glm::value_ptr(rectColor));
    }

    // Draw the rectangle
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void RenderComponent::cleanup() {
    if (glInitialized) {
        if (VAO != 0) {
            glDeleteVertexArrays(1, &VAO);
            VAO = 0;
        }
        if (VBO != 0) {
            glDeleteBuffers(1, &VBO);
            VBO = 0;
        }
        glInitialized = false;
    }
}

void RenderComponent::onAttach() {
    // Component attached
}

void RenderComponent::onDetach() {
    cleanup();
}
