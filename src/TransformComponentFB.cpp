#include "TransformComponentFB.h"
#include "GameEntity.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>
#include <algorithm>

TransformComponentFB::TransformComponentFB(const std::string& name)
    : Component(name) {
    // Allocate space in shared SOA storage
    auto storage = getSharedStorage();
    storageHandle = storage->allocate();
}

TransformComponentFB::~TransformComponentFB() {
    if (storageHandle != TransformDataStorage::INVALID_HANDLE) {
        auto storage = getSharedStorage();
        storage->deallocate(storageHandle);
    }
}

glm::vec3 TransformComponentFB::getLocalPosition() const {
    if (storageHandle == TransformDataStorage::INVALID_HANDLE) return glm::vec3(0.0f);
    return getSharedStorage()->getPosition(storageHandle);
}

void TransformComponentFB::setLocalPosition(const glm::vec3& pos) {
    if (storageHandle == TransformDataStorage::INVALID_HANDLE) return;
    getSharedStorage()->setPosition(storageHandle, pos);
    matrixDirty = true;
}

glm::quat TransformComponentFB::getLocalRotation() const {
    if (storageHandle == TransformDataStorage::INVALID_HANDLE) return glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    return getSharedStorage()->getRotation(storageHandle);
}

void TransformComponentFB::setLocalRotation(const glm::quat& rot) {
    if (storageHandle == TransformDataStorage::INVALID_HANDLE) return;
    getSharedStorage()->setRotation(storageHandle, glm::normalize(rot));
    matrixDirty = true;
}

glm::vec3 TransformComponentFB::getLocalScale() const {
    if (storageHandle == TransformDataStorage::INVALID_HANDLE) return glm::vec3(1.0f);
    return getSharedStorage()->getScale(storageHandle);
}

void TransformComponentFB::setLocalScale(const glm::vec3& scale) {
    if (storageHandle == TransformDataStorage::INVALID_HANDLE) return;
    getSharedStorage()->setScale(storageHandle, scale);
    matrixDirty = true;
}

void TransformComponentFB::setLocalTRS(const glm::vec3& pos, const glm::quat& rot, const glm::vec3& scale) {
    if (storageHandle == TransformDataStorage::INVALID_HANDLE) return;
    auto storage = getSharedStorage();
    storage->setPosition(storageHandle, pos);
    storage->setRotation(storageHandle, glm::normalize(rot));
    storage->setScale(storageHandle, scale);
    matrixDirty = true;
}

glm::mat4 TransformComponentFB::getLocalMatrix() const {
    if (matrixDirty) {
        const_cast<TransformComponentFB*>(this)->updateWorldMatrix();
    }
    return glm::mat4(1.0f); // Local matrix not stored, computed on demand
}

glm::mat4 TransformComponentFB::getWorldMatrix() const {
    if (storageHandle == TransformDataStorage::INVALID_HANDLE) return glm::mat4(1.0f);
    if (matrixDirty) {
        const_cast<TransformComponentFB*>(this)->updateWorldMatrix();
    }
    return getSharedStorage()->getWorldMatrix(storageHandle);
}

glm::vec3 TransformComponentFB::getWorldPosition() const {
    glm::mat4 world = getWorldMatrix();
    return glm::vec3(world[3]);
}

void TransformComponentFB::setWorldPosition(const glm::vec3& pos) {
    auto parent = parentEntity.lock();
    if (parent) {
        auto parentTransform = parent->getComponent<TransformComponentFB>();
        if (parentTransform) {
            glm::mat4 parentWorld = parentTransform->getWorldMatrix();
            glm::mat4 parentInv = glm::inverse(parentWorld);
            glm::vec4 localPos = parentInv * glm::vec4(pos, 1.0f);
            setLocalPosition(glm::vec3(localPos));
            return;
        }
    }
    setLocalPosition(pos);
}

glm::quat TransformComponentFB::getWorldRotation() const {
    auto parent = parentEntity.lock();
    if (parent) {
        auto parentTransform = parent->getComponent<TransformComponentFB>();
        if (parentTransform) {
            return glm::normalize(parentTransform->getWorldRotation() * getLocalRotation());
        }
    }
    return getLocalRotation();
}

void TransformComponentFB::setWorldRotation(const glm::quat& rot) {
    auto parent = parentEntity.lock();
    if (parent) {
        auto parentTransform = parent->getComponent<TransformComponentFB>();
        if (parentTransform) {
            setLocalRotation(glm::normalize(glm::inverse(parentTransform->getWorldRotation()) * rot));
            return;
        }
    }
    setLocalRotation(rot);
}

glm::vec3 TransformComponentFB::getWorldScale() const {
    auto parent = parentEntity.lock();
    if (parent) {
        auto parentTransform = parent->getComponent<TransformComponentFB>();
        if (parentTransform) {
            glm::vec3 parentScale = parentTransform->getWorldScale();
            return getLocalScale() * parentScale;
        }
    }
    return getLocalScale();
}

void TransformComponentFB::setWorldScale(const glm::vec3& scale) {
    auto parent = parentEntity.lock();
    if (parent) {
        auto parentTransform = parent->getComponent<TransformComponentFB>();
        if (parentTransform) {
            glm::vec3 parentScale = parentTransform->getWorldScale();
            setLocalScale(scale / parentScale);
            return;
        }
    }
    setLocalScale(scale);
}

void TransformComponentFB::setParent(std::shared_ptr<GameEntity> parent) {
    parentEntity = parent;
    if (storageHandle != TransformDataStorage::INVALID_HANDLE) {
        getSharedStorage()->setParent(storageHandle, 
            parent && parent->getComponent<TransformComponentFB>() ? 
            parent->getComponent<TransformComponentFB>()->storageHandle : 
            TransformDataStorage::INVALID_HANDLE);
    }
    matrixDirty = true;
}

void TransformComponentFB::addChild(std::shared_ptr<GameEntity> child) {
    childEntities.push_back(child);
}

void TransformComponentFB::removeChild(std::shared_ptr<GameEntity> child) {
    auto it = std::find(childEntities.begin(), childEntities.end(), child);
    if (it != childEntities.end()) {
        childEntities.erase(it);
    }
}

void TransformComponentFB::updateWorldMatrix() {
    if (storageHandle == TransformDataStorage::INVALID_HANDLE) return;
    
    auto storage = getSharedStorage();
    
    // Build local matrix
    glm::vec3 localPos = storage->getPosition(storageHandle);
    glm::quat localRot = storage->getRotation(storageHandle);
    glm::vec3 localScl = storage->getScale(storageHandle);
    
    glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), localPos);
    glm::mat4 rotationMatrix = glm::mat4_cast(localRot);
    glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), localScl);
    
    glm::mat4 localMatrix = translationMatrix * rotationMatrix * scaleMatrix;
    
    // Calculate world matrix
    auto parent = parentEntity.lock();
    if (parent) {
        auto parentTransform = parent->getComponent<TransformComponentFB>();
        if (parentTransform && parentTransform->storageHandle != TransformDataStorage::INVALID_HANDLE) {
            glm::mat4 parentWorld = parentTransform->getWorldMatrix();
            storage->setWorldMatrix(storageHandle, parentWorld * localMatrix);
        } else {
            storage->setWorldMatrix(storageHandle, localMatrix);
        }
    } else {
        storage->setWorldMatrix(storageHandle, localMatrix);
    }
    
    matrixDirty = false;
}

void TransformComponentFB::onUpdate(float deltaTime) {
    if (matrixDirty) {
        updateWorldMatrix();
    }
}

void TransformComponentFB::onAttach() {
    // Component attached to entity
}

void TransformComponentFB::onDetach() {
    // Component detached from entity
    parentEntity.reset();
    childEntities.clear();
}
