#include "RenderComponent.h"

RenderComponent::RenderComponent(const std::string& name)
    : Component(name) {
}

void RenderComponent::onAttach() {
    // Component attached - no OpenGL initialization needed
    // RenderSystem handles all rendering
}

void RenderComponent::onDetach() {
    // Component detached - no cleanup needed
}

