#include "CollisionComponent.h"
#include <iostream>

// 静态成员初始化
std::shared_ptr<CollisionDataStorage> CollisionComponent::s_sharedStorage = 
    std::make_shared<CollisionDataStorage>();

CollisionComponent::CollisionComponent(const std::string& name)
    : EntityComponent(name) {
    // 在 SOA 存储中分配槽位
    storageHandle = s_sharedStorage->allocate();
    
    // 设置默认值
    s_sharedStorage->setBoundingBox(storageHandle, glm::vec3(-1.0f), glm::vec3(1.0f));
    s_sharedStorage->setCollisionLayer(storageHandle, 0);
    s_sharedStorage->setCollisionMask(storageHandle, 0xFFFFFFFF);
    s_sharedStorage->setEnabled(storageHandle, true);
}

CollisionComponent::~CollisionComponent() {
    // 释放 SOA 存储槽位
    if (storageHandle.isValid()) {
        s_sharedStorage->deallocate(storageHandle);
    }
}

void CollisionComponent::setBoundingBox(const glm::vec3& min, const glm::vec3& max) {
    s_sharedStorage->setBoundingBox(storageHandle, min, max);
}

glm::vec3 CollisionComponent::getBoundingBoxMin() const {
    return s_sharedStorage->getBoundingBoxMin(storageHandle);
}

glm::vec3 CollisionComponent::getBoundingBoxMax() const {
    return s_sharedStorage->getBoundingBoxMax(storageHandle);
}

void CollisionComponent::setCollisionLayer(uint32_t layer) {
    s_sharedStorage->setCollisionLayer(storageHandle, layer);
}

uint32_t CollisionComponent::getCollisionLayer() const {
    return s_sharedStorage->getCollisionLayer(storageHandle);
}

void CollisionComponent::setCollisionMask(uint32_t mask) {
    s_sharedStorage->setCollisionMask(storageHandle, mask);
}

uint32_t CollisionComponent::getCollisionMask() const {
    return s_sharedStorage->getCollisionMask(storageHandle);
}

void CollisionComponent::setEnabled(bool enable) {
    s_sharedStorage->setEnabled(storageHandle, enable);
}

bool CollisionComponent::isEnabled() const {
    return s_sharedStorage->isEnabled(storageHandle);
}

std::shared_ptr<CollisionDataStorage> CollisionComponent::getSharedStorage() {
    return s_sharedStorage;
}

void CollisionComponent::onAttach() {
    // EntityComponent attached
}

void CollisionComponent::onDetach() {
    // EntityComponent detached
}
