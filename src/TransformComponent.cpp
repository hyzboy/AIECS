#include "TransformComponent.h"
#include "GameEntity.h"
#include <glm/gtx/euler_angles.hpp>
#include <algorithm>

TransformComponent::TransformComponent(const std::string& name)
    : Component(name) {
    // Allocate space in shared SOA storage
    auto storage = getSharedStorage();
    storageHandle = storage->allocate();
}

TransformComponent::~TransformComponent() {
    if (storageHandle != TransformDataStorage::INVALID_HANDLE) {
        auto storage = getSharedStorage();
        storage->deallocate(storageHandle);
    }
}

glm::vec3 TransformComponent::getLocalPosition() const {
    if (storageHandle == TransformDataStorage::INVALID_HANDLE) return glm::vec3(0.0f);
    return getSharedStorage()->getPosition(storageHandle);
}

void TransformComponent::setLocalPosition(const glm::vec3& pos) {
    if (storageHandle == TransformDataStorage::INVALID_HANDLE) return;
    getSharedStorage()->setPosition(storageHandle, pos);
    
    // Static objects should not be modified after initialization
    if (mobility != TransformMobility::Static) {
        matrixDirty = true;
    }
}

glm::quat TransformComponent::getLocalRotation() const {
    if (storageHandle == TransformDataStorage::INVALID_HANDLE) return glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    return getSharedStorage()->getRotation(storageHandle);
}

void TransformComponent::setLocalRotation(const glm::quat& rot) {
    if (storageHandle == TransformDataStorage::INVALID_HANDLE) return;
    getSharedStorage()->setRotation(storageHandle, glm::normalize(rot));
    
    if (mobility != TransformMobility::Static) {
        matrixDirty = true;
    }
}

glm::vec3 TransformComponent::getLocalScale() const {
    if (storageHandle == TransformDataStorage::INVALID_HANDLE) return glm::vec3(1.0f);
    return getSharedStorage()->getScale(storageHandle);
}

void TransformComponent::setLocalScale(const glm::vec3& scale) {
    if (storageHandle == TransformDataStorage::INVALID_HANDLE) return;
    getSharedStorage()->setScale(storageHandle, scale);
    
    if (mobility != TransformMobility::Static) {
        matrixDirty = true;
    }
}

void TransformComponent::setLocalTRS(const glm::vec3& pos, const glm::quat& rot, const glm::vec3& scale) {
    if (storageHandle == TransformDataStorage::INVALID_HANDLE) return;
    auto storage = getSharedStorage();
    storage->setPosition(storageHandle, pos);
    storage->setRotation(storageHandle, glm::normalize(rot));
    storage->setScale(storageHandle, scale);
    
    if (mobility != TransformMobility::Static) {
        matrixDirty = true;
    }
}

glm::mat4 TransformComponent::getLocalMatrix() const {
    if (matrixDirty) {
        const_cast<TransformComponent*>(this)->updateWorldMatrix();
    }
    return glm::mat4(1.0f); // Local matrix not stored, computed on demand
}

glm::mat4 TransformComponent::getWorldMatrix() const {
    if (storageHandle == TransformDataStorage::INVALID_HANDLE) return glm::mat4(1.0f);
    
    // For Static objects, use cached matrix (never update)
    if (mobility == TransformMobility::Static && !matrixDirty) {
        return cachedWorldMatrix;
    }
    
    // For Movable objects or when dirty, always recalculate
    if (matrixDirty) {
        const_cast<TransformComponent*>(this)->updateWorldMatrix();
    }
    
    return getSharedStorage()->getWorldMatrix(storageHandle);
}

glm::vec3 TransformComponent::getWorldPosition() const {
    glm::mat4 world = getWorldMatrix();
    return glm::vec3(world[3]);
}

void TransformComponent::setWorldPosition(const glm::vec3& pos) {
    auto parent = parentEntity.lock();
    if (parent) {
        auto parentTransform = parent->getComponent<TransformComponent>();
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

glm::quat TransformComponent::getWorldRotation() const {
    auto parent = parentEntity.lock();
    if (parent) {
        auto parentTransform = parent->getComponent<TransformComponent>();
        if (parentTransform) {
            return glm::normalize(parentTransform->getWorldRotation() * getLocalRotation());
        }
    }
    return getLocalRotation();
}

void TransformComponent::setWorldRotation(const glm::quat& rot) {
    auto parent = parentEntity.lock();
    if (parent) {
        auto parentTransform = parent->getComponent<TransformComponent>();
        if (parentTransform) {
            setLocalRotation(glm::normalize(glm::inverse(parentTransform->getWorldRotation()) * rot));
            return;
        }
    }
    setLocalRotation(rot);
}

glm::vec3 TransformComponent::getWorldScale() const {
    auto parent = parentEntity.lock();
    if (parent) {
        auto parentTransform = parent->getComponent<TransformComponent>();
        if (parentTransform) {
            glm::vec3 parentScale = parentTransform->getWorldScale();
            return getLocalScale() * parentScale;
        }
    }
    return getLocalScale();
}

void TransformComponent::setWorldScale(const glm::vec3& scale) {
    auto parent = parentEntity.lock();
    if (parent) {
        auto parentTransform = parent->getComponent<TransformComponent>();
        if (parentTransform) {
            glm::vec3 parentScale = parentTransform->getWorldScale();
            setLocalScale(scale / parentScale);
            return;
        }
    }
    setLocalScale(scale);
}

void TransformComponent::setParent(std::shared_ptr<GameEntity> parent) {
    parentEntity = parent;
    if (storageHandle != TransformDataStorage::INVALID_HANDLE) {
        getSharedStorage()->setParent(storageHandle, 
            parent && parent->getComponent<TransformComponent>() ? 
            parent->getComponent<TransformComponent>()->storageHandle : 
            TransformDataStorage::INVALID_HANDLE);
    }
    matrixDirty = true;
}

void TransformComponent::addChild(std::shared_ptr<GameEntity> child) {
    childEntities.push_back(child);
}

void TransformComponent::removeChild(std::shared_ptr<GameEntity> child) {
    auto it = std::find(childEntities.begin(), childEntities.end(), child);
    if (it != childEntities.end()) {
        childEntities.erase(it);
    }
}

void TransformComponent::setMobility(TransformMobility newMobility) {
    mobility = newMobility;
    
    // Sync mobility to SOA storage
    if (storageHandle != TransformDataStorage::INVALID_HANDLE) {
        auto storage = getSharedStorage();
        storage->setMobility(storageHandle, static_cast<uint8_t>(mobility == TransformMobility::Static ? 0 : 1));
    }
    
    // When setting to Static, cache the current world matrix
    if (mobility == TransformMobility::Static) {
        if (matrixDirty) {
            updateWorldMatrix();
        }
        cachedWorldMatrix = getSharedStorage()->getWorldMatrix(storageHandle);
        matrixDirty = false;  // Static objects never become dirty
    }
}

void TransformComponent::updateWorldMatrix() {
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
        auto parentTransform = parent->getComponent<TransformComponent>();
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
    
    // Cache the world matrix for Static objects
    if (mobility == TransformMobility::Static) {
        cachedWorldMatrix = storage->getWorldMatrix(storageHandle);
    }
}

void TransformComponent::onUpdate(float deltaTime) {
    if (matrixDirty) {
        updateWorldMatrix();
    }
}

void TransformComponent::onAttach() {
    // Component attached to entity
}

void TransformComponent::onDetach() {
    // Component detached from entity
    parentEntity.reset();
    childEntities.clear();
}
