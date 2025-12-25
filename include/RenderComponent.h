#pragma once

#include "GameEntity.h"
#include "Material.h"
#include <glm/glm.hpp>
#include <string>

/// Render component for 2D rectangle rendering with OpenGL
/// Stores rendering data only - actual rendering is done by RenderSystem
class RenderComponent : public EntityComponent {
public:
    RenderComponent(const std::string& name = "Render");
    ~RenderComponent() override = default;

    // Material accessors
    void setMaterial(MaterialPtr mat) { material = mat; }
    MaterialPtr getMaterial() const { return material; }

    // Convenience color accessors (creates new material if needed)
    void setColor(const glm::vec4& color);
    glm::vec4 getColor() const;

    void setVisible(bool visible) { isVisible = visible; }
    bool getVisible() const { return isVisible; }

    void onAttach() override;
    void onDetach() override;

private:
    MaterialPtr material;
    bool isVisible = true;
};
