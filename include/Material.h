#pragma once

#include <glm/glm.hpp>
#include <memory>

/// Material mutability type (similar to transform mobility)
enum class MaterialMutability {
    Static,   // Material never changes, upload once
    Dynamic   // Material can change frequently, update each frame
};

/// Material class for rendering
/// Currently stores only color, but can be extended with textures, shaders, etc.
class Material {
public:
    Material(const glm::vec4& color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
             MaterialMutability mutability = MaterialMutability::Static)
        : color(color), mutability(mutability) {}

    ~Material() = default;

    // Color accessors
    void setColor(const glm::vec4& newColor) { color = newColor; }
    const glm::vec4& getColor() const { return color; }

    // Mutability accessors
    void setMutability(MaterialMutability mut) { mutability = mut; }
    MaterialMutability getMutability() const { return mutability; }
    bool isStatic() const { return mutability == MaterialMutability::Static; }
    bool isDynamic() const { return mutability == MaterialMutability::Dynamic; }

    // Equality comparison for deduplication
    bool operator==(const Material& other) const {
        return color == other.color;
    }

    bool operator!=(const Material& other) const {
        return !(*this == other);
    }

private:
    glm::vec4 color;
    MaterialMutability mutability;
};

using MaterialPtr = std::shared_ptr<Material>;
