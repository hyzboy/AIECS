#pragma once

#include <cstdint>
#include <string>
#include <memory>

/// Base class for all objects in the Frostbite-style engine
/// Provides fundamental object lifetime and identification
class Object {
public:
    using ObjectID = uint64_t;
    static constexpr ObjectID INVALID_OBJECT_ID = 0;

    explicit Object(const std::string& name = "Object");
    virtual ~Object() = default;

    // Delete copy operations
    Object(const Object&) = delete;
    Object& operator=(const Object&) = delete;

    // Allow move operations
    Object(Object&&) noexcept = default;
    Object& operator=(Object&&) noexcept = default;

    /// Get unique object ID
    ObjectID getID() const { return objectId; }

    /// Get object name
    const std::string& getName() const { return objectName; }

    /// Set object name
    void setName(const std::string& name) { objectName = name; }

    /// Check if object is valid
    bool isValid() const { return objectId != INVALID_OBJECT_ID; }

    /// Called when object is created
    virtual void onCreate() {}

    /// Called every frame for updates
    virtual void onUpdate(float deltaTime) {}

    /// Called when object is destroyed
    virtual void onDestroy() {}

protected:
    ObjectID objectId;
    std::string objectName;

private:
    static ObjectID nextObjectId;
};
