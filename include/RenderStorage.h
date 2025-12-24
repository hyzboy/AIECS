#pragma once

#include "ComponentTypes.h"
#include <vector>
#include <string>

/// SOA storage for Render components
class RenderStorage {
public:
    RenderStorage() = default;

    /// Allocate a new render component slot
    ComponentID allocate() {
        ComponentID compId;
        if (!freeList.empty()) {
            compId = freeList.back();
            freeList.pop_back();
            
            // Reset data
            meshName[compId] = "";
            materialName[compId] = "";
            isVisible[compId] = true;
            castShadows[compId] = true;
        } else {
            compId = static_cast<ComponentID>(meshName.size());
            meshName.emplace_back("");
            materialName.emplace_back("");
            isVisible.emplace_back(true);
            castShadows.emplace_back(true);
        }
        return compId;
    }

    /// Free a render component slot
    void deallocate(ComponentID compId) {
        if (compId < meshName.size()) {
            freeList.push_back(compId);
        }
    }

    /// Check if component ID is valid
    bool isValid(ComponentID compId) const {
        if (compId >= meshName.size()) return false;
        return std::find(freeList.begin(), freeList.end(), compId) == freeList.end();
    }

    /// Get mesh name
    const std::string& getMeshName(ComponentID compId) const {
        return meshName[compId];
    }

    void setMeshName(ComponentID compId, const std::string& name) {
        meshName[compId] = name;
    }

    /// Get material name
    const std::string& getMaterialName(ComponentID compId) const {
        return materialName[compId];
    }

    void setMaterialName(ComponentID compId, const std::string& name) {
        materialName[compId] = name;
    }

    /// Check if visible
    bool getVisible(ComponentID compId) const {
        return isVisible[compId];
    }

    void setVisible(ComponentID compId, bool visible) {
        isVisible[compId] = visible;
    }

    /// Check if casts shadows
    bool getCastShadows(ComponentID compId) const {
        return castShadows[compId];
    }

    void setCastShadows(ComponentID compId, bool shadows) {
        castShadows[compId] = shadows;
    }

    size_t size() const {
        return meshName.size();
    }

private:
    std::vector<std::string> meshName;
    std::vector<std::string> materialName;
    std::vector<bool> isVisible;
    std::vector<bool> castShadows;
    std::vector<ComponentID> freeList;
};
