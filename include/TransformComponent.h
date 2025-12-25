#pragma once

#include "GameEntity.h"
#include "TransformDataStorage.h"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>

/// Mobility type for transform optimization (similar to Unreal Engine)
enum class TransformMobility {
    Static,      // Never moves, transform matrix cached permanently
    Stationary,  // Rarely moves, transform updated only when explicitly changed
    Movable      // Frequently moves, transform updated every frame
};

/// Transform component for Frostbite architecture
/// Uses SOA (Structure of Arrays) storage for better cache performance
/// while maintaining OOP component interface
class TransformComponent : public Component {
public:
    TransformComponent(const std::string& name = "Transform");
    ~TransformComponent() override;

    // Local transform accessors (using SOA backend)
    glm::vec3 getLocalPosition() const;
    void setLocalPosition(const glm::vec3& pos);

    glm::quat getLocalRotation() const;
    void setLocalRotation(const glm::quat& rot);

    glm::vec3 getLocalScale() const;
    void setLocalScale(const glm::vec3& scale);

    void setLocalTRS(const glm::vec3& pos, const glm::quat& rot, const glm::vec3& scale);

    // World transform accessors
    glm::mat4 getLocalMatrix() const;
    glm::mat4 getWorldMatrix() const;

    glm::vec3 getWorldPosition() const;
    void setWorldPosition(const glm::vec3& pos);

    glm::quat getWorldRotation() const;
    void setWorldRotation(const glm::quat& rot);

    glm::vec3 getWorldScale() const;
    void setWorldScale(const glm::vec3& scale);

    // Parent/Child relationships
    void setParent(std::shared_ptr<GameEntity> parent);
    std::shared_ptr<GameEntity> getParent() const { return parentEntity.lock(); }

    void addChild(std::shared_ptr<GameEntity> child);
    void removeChild(std::shared_ptr<GameEntity> child);
    const std::vector<std::shared_ptr<GameEntity>>& getChildren() const { return childEntities; }

    // Mobility settings for optimization
    void setMobility(TransformMobility mobility);
    TransformMobility getMobility() const { return mobility; }
    bool isStatic() const { return mobility == TransformMobility::Static; }
    bool isStationary() const { return mobility == TransformMobility::Stationary; }
    bool isMovable() const { return mobility == TransformMobility::Movable; }

    // Mark transform as dirty (for Stationary objects)
    void markDirty() { matrixDirty = true; }

    void onUpdate(float deltaTime) override;
    void onAttach() override;
    void onDetach() override;

    // Get the SOA storage handle for batch operations
    TransformDataStorage::HandleID getStorageHandle() const { return storageHandle; }

    // Static access to storage for batch operations
    static std::shared_ptr<TransformDataStorage>& getSharedStorage() {
        static auto storage = std::make_shared<TransformDataStorage>();
        return storage;
    }

private:
    void updateWorldMatrix();

    TransformDataStorage::HandleID storageHandle = TransformDataStorage::INVALID_HANDLE;
    std::weak_ptr<GameEntity> parentEntity;
    std::vector<std::shared_ptr<GameEntity>> childEntities;
    bool matrixDirty = true;
    TransformMobility mobility = TransformMobility::Movable;  // Default to movable
    glm::mat4 cachedWorldMatrix;  // Cached for Static/Stationary objects
};
