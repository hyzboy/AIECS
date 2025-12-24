#include "CollisionComponentFB.h"
#include <iostream>

// 静态成员初始化
std::shared_ptr<CollisionDataStorage> CollisionComponentFB::s_sharedStorage = 
    std::make_shared<CollisionDataStorage>();

CollisionComponentFB::CollisionComponentFB(const std::string& name)
    : Component(name) {
    // 在 SOA 存储中分配槽位
    storageHandle = s_sharedStorage->allocate();
    
    // 设置默认值
    s_sharedStorage->setBoundingBox(storageHandle, glm::vec3(-1.0f), glm::vec3(1.0f));
    s_sharedStorage->setCollisionLayer(storageHandle, 0);
    s_sharedStorage->setCollisionMask(storageHandle, 0xFFFFFFFF);
    s_sharedStorage->setEnabled(storageHandle, true);
}

CollisionComponentFB::~CollisionComponentFB() {
    // 释放 SOA 存储槽位
    if (storageHandle.isValid()) {
        s_sharedStorage->deallocate(storageHandle);
    }
}

void CollisionComponentFB::setBoundingBox(const glm::vec3& min, const glm::vec3& max) {
    s_sharedStorage->setBoundingBox(storageHandle, min, max);
}

glm::vec3 CollisionComponentFB::getBoundingBoxMin() const {
    return s_sharedStorage->getBoundingBoxMin(storageHandle);
}

glm::vec3 CollisionComponentFB::getBoundingBoxMax() const {
    return s_sharedStorage->getBoundingBoxMax(storageHandle);
}

void CollisionComponentFB::setCollisionLayer(uint32_t layer) {
    s_sharedStorage->setCollisionLayer(storageHandle, layer);
}

uint32_t CollisionComponentFB::getCollisionLayer() const {
    return s_sharedStorage->getCollisionLayer(storageHandle);
}

void CollisionComponentFB::setCollisionMask(uint32_t mask) {
    s_sharedStorage->setCollisionMask(storageHandle, mask);
}

uint32_t CollisionComponentFB::getCollisionMask() const {
    return s_sharedStorage->getCollisionMask(storageHandle);
}

void CollisionComponentFB::setEnabled(bool enable) {
    s_sharedStorage->setEnabled(storageHandle, enable);
}

bool CollisionComponentFB::isEnabled() const {
    return s_sharedStorage->isEnabled(storageHandle);
}

std::shared_ptr<CollisionDataStorage> CollisionComponentFB::getSharedStorage() {
    return s_sharedStorage;
}

void CollisionComponentFB::onAttach() {
    // Component attached
}

void CollisionComponentFB::onDetach() {
    // Component detached
}
