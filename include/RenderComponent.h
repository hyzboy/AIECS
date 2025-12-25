#pragma once

#include "GameEntity.h"
#include <glm/glm.hpp>
#include <string>

/// Render component for 2D rectangle rendering with OpenGL
/// Renders a rectangle from -0.5,-0.5 to +0.5,+0.5
/// Uses TransformComponent for positioning, rotation, and scaling
class RenderComponent : public Component {
public:
    RenderComponent(const std::string& name = "Render");
    ~RenderComponent() override;

    void setColor(const glm::vec4& color) { rectColor = color; }
    const glm::vec4& getColor() const { return rectColor; }

    void setVisible(bool visible) { isVisible = visible; }
    bool getVisible() const { return isVisible; }

    // Initialize OpenGL resources (VAO, VBO)
    void initializeGL();
    
    // Render the rectangle using the entity's TransformComponent
    void render(unsigned int shaderProgram, GameEntity* entity);

    void onAttach() override;
    void onDetach() override;

    // Cleanup OpenGL resources
    void cleanup();

private:
    glm::vec4 rectColor = glm::vec4(0.2f, 0.6f, 0.8f, 1.0f); // Default blue
    bool isVisible = true;
    
    // OpenGL resources
    unsigned int VAO = 0;
    unsigned int VBO = 0;
    bool glInitialized = false;
};
