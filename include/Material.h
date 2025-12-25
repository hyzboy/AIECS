#pragma once

#include <glm/glm.hpp>
#include <memory>

/// Material class for rendering
/// Currently stores only color, but can be extended with textures, shaders, etc.
class Material {
public:
    Material(const glm::vec4& color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f))
        : color(color) {}

    ~Material() = default;

    // Color accessors
    void setColor(const glm::vec4& newColor) { color = newColor; }
    const glm::vec4& getColor() const { return color; }

    // Equality comparison for deduplication
    bool operator==(const Material& other) const {
        return color == other.color;
    }

    bool operator!=(const Material& other) const {
        return !(*this == other);
    }

private:
    glm::vec4 color;
};

using MaterialPtr = std::shared_ptr<Material>;
