#pragma once

#include "GameEntity.h"
#include <glm/glm.hpp>
#include <string>

/// Render component for 2D rectangle rendering with OpenGL
/// Stores rendering data only - actual rendering is done by RenderSystem
class RenderComponent : public Component {
public:
    RenderComponent(const std::string& name = "Render");
    ~RenderComponent() override = default;

    void setColor(const glm::vec4& color) { rectColor = color; }
    const glm::vec4& getColor() const { return rectColor; }

    void setVisible(bool visible) { isVisible = visible; }
    bool getVisible() const { return isVisible; }

    void onAttach() override;
    void onDetach() override;

private:
    glm::vec4 rectColor = glm::vec4(0.2f, 0.6f, 0.8f, 1.0f); // Default blue
    bool isVisible = true;
};
