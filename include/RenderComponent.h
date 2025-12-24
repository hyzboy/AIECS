#pragma once

#include "RenderStorage.h"
#include "ComponentTypes.h"
#include <string>

/// RenderComponent accessor for reading and writing render data
class RenderComponent {
public:
    RenderComponent(ComponentID compId, RenderStorage* storage)
        : componentId(compId), renderStorage(storage) {}

    /// Check if this component is valid
    bool isValid() const {
        return componentId != INVALID_COMPONENT && 
               renderStorage != nullptr && 
               renderStorage->isValid(componentId);
    }

    /// Get component ID
    ComponentID getComponentID() const {
        return componentId;
    }

    /// Get mesh name
    std::string getMeshName() const {
        if (!isValid()) return "";
        return renderStorage->getMeshName(componentId);
    }

    void setMeshName(const std::string& name) {
        if (isValid()) {
            renderStorage->setMeshName(componentId, name);
        }
    }

    /// Get material name
    std::string getMaterialName() const {
        if (!isValid()) return "";
        return renderStorage->getMaterialName(componentId);
    }

    void setMaterialName(const std::string& name) {
        if (isValid()) {
            renderStorage->setMaterialName(componentId, name);
        }
    }

    /// Check if visible
    bool isVisible() const {
        if (!isValid()) return false;
        return renderStorage->getVisible(componentId);
    }

    void setVisible(bool visible) {
        if (isValid()) {
            renderStorage->setVisible(componentId, visible);
        }
    }

    /// Check if casts shadows
    bool castsShadows() const {
        if (!isValid()) return false;
        return renderStorage->getCastShadows(componentId);
    }

    void setCastShadows(bool shadows) {
        if (isValid()) {
            renderStorage->setCastShadows(componentId, shadows);
        }
    }

private:
    ComponentID componentId;
    RenderStorage* renderStorage;
};
