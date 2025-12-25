#include "RenderComponent.h"

RenderComponent::RenderComponent(const std::string& name)
    : EntityComponent(name) {
    // Create default material with blue color
    material = std::make_shared<Material>(glm::vec4(0.2f, 0.6f, 0.8f, 1.0f));
}

void RenderComponent::setColor(const glm::vec4& color) {
    if (!material) {
        material = std::make_shared<Material>(color);
    } else {
        material->setColor(color);
    }
}

glm::vec4 RenderComponent::getColor() const {
    if (material) {
        return material->getColor();
    }
    return glm::vec4(1.0f); // White default
}

void RenderComponent::onAttach() {
    // EntityComponent attached - no OpenGL initialization needed
    // RenderSystem handles all rendering
}

void RenderComponent::onDetach() {
    // EntityComponent detached - no cleanup needed
}

