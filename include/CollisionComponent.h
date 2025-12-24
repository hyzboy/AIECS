#pragma once

#include "GameEntity.h"
#include "CollisionDataStorage.h"
#include <glm/glm.hpp>
#include <memory>

/// Collision component for Frostbite architecture (with SOA backend)
class CollisionComponent : public Component {
public:
    CollisionComponent(const std::string& name = "Collision");
    ~CollisionComponent() override;

    void setBoundingBox(const glm::vec3& min, const glm::vec3& max);
    glm::vec3 getBoundingBoxMin() const;
    glm::vec3 getBoundingBoxMax() const;

    void setCollisionLayer(uint32_t layer);
    uint32_t getCollisionLayer() const;

    void setCollisionMask(uint32_t mask);
    uint32_t getCollisionMask() const;

    void setEnabled(bool enable);
    bool isEnabled() const;

    void onAttach() override;
    void onDetach() override;

    // === SOA 后端访问 ===
    // 获取共享的 SOA 存储（用于批量处理）
    static std::shared_ptr<CollisionDataStorage> getSharedStorage();

private:
    // 使用 handle 访问 SOA 后端存储
    CollisionDataStorage::HandleID storageHandle;
    
    // 共享的 SOA 存储（所有 CollisionComponentFB 实例共享）
    static std::shared_ptr<CollisionDataStorage> s_sharedStorage;
};
