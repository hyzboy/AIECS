#include "CollisionComponentFB.h"

CollisionComponentFB::CollisionComponentFB(const std::string& name)
    : Component(name) {
}

void CollisionComponentFB::setBoundingBox(const glm::vec3& min, const glm::vec3& max) {
    boundingBoxMin = min;
    boundingBoxMax = max;
}

void CollisionComponentFB::onAttach() {
    // Component attached
}

void CollisionComponentFB::onDetach() {
    // Component detached
}
