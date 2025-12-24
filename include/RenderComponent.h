#pragma once

#include "GameEntity.h"
#include <string>

/// Render component for Frostbite architecture
class RenderComponent : public Component {
public:
    RenderComponent(const std::string& name = "Render");
    ~RenderComponent() override = default;

    void setMeshName(const std::string& mesh) { meshName = mesh; }
    const std::string& getMeshName() const { return meshName; }

    void setMaterialName(const std::string& material) { materialName = material; }
    const std::string& getMaterialName() const { return materialName; }

    void setVisible(bool visible) { isVisible = visible; }
    bool getVisible() const { return isVisible; }

    void setCastShadows(bool cast) { castsShadows = cast; }
    bool getCastShadows() const { return castsShadows; }

    void onAttach() override;
    void onDetach() override;

private:
    std::string meshName;
    std::string materialName;
    bool isVisible = true;
    bool castsShadows = true;
};
