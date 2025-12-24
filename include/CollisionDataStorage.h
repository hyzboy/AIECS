#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <cstdint>

/**
 * @brief SOA (Structure of Arrays) 存储碰撞数据
 * 
 * 将碰撞数据按字段分离存储，优化缓存局部性
 * 适用于批量碰撞检测、空间查询等操作
 */
class CollisionDataStorage {
public:
    struct HandleID {
        size_t index = 0;
        bool isValid() const { return index > 0; }
    };

    CollisionDataStorage() = default;

    // 分配新的碰撞数据槽位
    HandleID allocate() {
        HandleID handle;
        handle.index = boundingBoxMins.size();
        
        boundingBoxMins.emplace_back(glm::vec3(0.0f));
        boundingBoxMaxs.emplace_back(glm::vec3(0.0f));
        collisionLayers.emplace_back(0);
        collisionMasks.emplace_back(0xFFFFFFFF);
        enabledFlags.emplace_back(true);
        
        return handle;
    }

    // 移除碰撞数据（简单实现：设为无效）
    void deallocate(HandleID handle) {
        if (handle.isValid() && handle.index < enabledFlags.size()) {
            enabledFlags[handle.index] = false;
        }
    }

    // 获取包围盒最小点
    glm::vec3 getBoundingBoxMin(HandleID handle) const {
        return boundingBoxMins[handle.index];
    }

    // 设置包围盒最小点
    void setBoundingBoxMin(HandleID handle, const glm::vec3& min) {
        boundingBoxMins[handle.index] = min;
    }

    // 获取包围盒最大点
    glm::vec3 getBoundingBoxMax(HandleID handle) const {
        return boundingBoxMaxs[handle.index];
    }

    // 设置包围盒最大点
    void setBoundingBoxMax(HandleID handle, const glm::vec3& max) {
        boundingBoxMaxs[handle.index] = max;
    }

    // 设置包围盒（同时设置最小和最大点）
    void setBoundingBox(HandleID handle, const glm::vec3& min, const glm::vec3& max) {
        boundingBoxMins[handle.index] = min;
        boundingBoxMaxs[handle.index] = max;
    }

    // 获取碰撞层
    uint32_t getCollisionLayer(HandleID handle) const {
        return collisionLayers[handle.index];
    }

    // 设置碰撞层
    void setCollisionLayer(HandleID handle, uint32_t layer) {
        collisionLayers[handle.index] = layer;
    }

    // 获取碰撞掩码
    uint32_t getCollisionMask(HandleID handle) const {
        return collisionMasks[handle.index];
    }

    // 设置碰撞掩码
    void setCollisionMask(HandleID handle, uint32_t mask) {
        collisionMasks[handle.index] = mask;
    }

    // 获取启用状态
    bool isEnabled(HandleID handle) const {
        return enabledFlags[handle.index];
    }

    // 设置启用状态
    void setEnabled(HandleID handle, bool enabled) {
        enabledFlags[handle.index] = enabled;
    }

    // === 批量访问接口（用于高性能批处理） ===

    // 获取所有包围盒最小点（直接访问 SOA 数组）
    std::vector<glm::vec3>& getAllBoundingBoxMins() { return boundingBoxMins; }
    const std::vector<glm::vec3>& getAllBoundingBoxMins() const { return boundingBoxMins; }

    // 获取所有包围盒最大点
    std::vector<glm::vec3>& getAllBoundingBoxMaxs() { return boundingBoxMaxs; }
    const std::vector<glm::vec3>& getAllBoundingBoxMaxs() const { return boundingBoxMaxs; }

    // 获取所有碰撞层
    std::vector<uint32_t>& getAllCollisionLayers() { return collisionLayers; }
    const std::vector<uint32_t>& getAllCollisionLayers() const { return collisionLayers; }

    // 获取所有碰撞掩码
    std::vector<uint32_t>& getAllCollisionMasks() { return collisionMasks; }
    const std::vector<uint32_t>& getAllCollisionMasks() const { return collisionMasks; }

    // 获取所有启用标志
    std::vector<bool>& getAllEnabledFlags() { return enabledFlags; }
    const std::vector<bool>& getAllEnabledFlags() const { return enabledFlags; }

    // 获取碰撞体数量
    size_t getCount() const { return boundingBoxMins.size(); }

    // 清空所有数据
    void clear() {
        boundingBoxMins.clear();
        boundingBoxMaxs.clear();
        collisionLayers.clear();
        collisionMasks.clear();
        enabledFlags.clear();
    }

    // 获取内存占用（字节）
    size_t getMemoryUsage() const {
        return boundingBoxMins.capacity() * sizeof(glm::vec3) +
               boundingBoxMaxs.capacity() * sizeof(glm::vec3) +
               collisionLayers.capacity() * sizeof(uint32_t) +
               collisionMasks.capacity() * sizeof(uint32_t) +
               enabledFlags.capacity() * sizeof(bool);
    }

private:
    // SOA 数据布局 - 每个属性单独存储在连续数组中
    std::vector<glm::vec3> boundingBoxMins;   // 包围盒最小点
    std::vector<glm::vec3> boundingBoxMaxs;   // 包围盒最大点
    std::vector<uint32_t> collisionLayers;    // 碰撞层（用于分组）
    std::vector<uint32_t> collisionMasks;     // 碰撞掩码（用于过滤）
    std::vector<bool> enabledFlags;           // 启用标志
};
