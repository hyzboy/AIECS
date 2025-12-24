#pragma once

#include "GameEntity.h"
#include <glm/glm.hpp>

/// Collision component for Frostbite architecture
class CollisionComponentFB : public Component {
public:
    CollisionComponentFB(const std::string& name = "Collision");
    ~CollisionComponentFB() override = default;

    void setBoundingBox(const glm::vec3& min, const glm::vec3& max);
    glm::vec3 getBoundingBoxMin() const { return boundingBoxMin; }
    glm::vec3 getBoundingBoxMax() const { return boundingBoxMax; }

    void setCollisionLayer(uint32_t layer) { collisionLayer = layer; }
    uint32_t getCollisionLayer() const { return collisionLayer; }

    void setEnabled(bool enable) { enabled = enable; }
    bool isEnabled() const { return enabled; }

    void onAttach() override;
    void onDetach() override;

private:
    glm::vec3 boundingBoxMin = glm::vec3(-1.0f);
    glm::vec3 boundingBoxMax = glm::vec3(1.0f);
    uint32_t collisionLayer = 0;
    bool enabled = true;
};
