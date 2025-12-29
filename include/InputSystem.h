#pragma once

#include "EntitySystem.h"
#include "World.h"
#include <memory>

// Forward declaration
struct GLFWwindow;

/// System that manages input processing for entities with InputComponent
/// Following ECS pattern: System iterates over entities with specific components
/// and processes their input events each frame
class InputSystem : public EntitySystem {
public:
    explicit InputSystem(const std::string& name = "InputSystem");
    ~InputSystem() override;

    void initialize() override;
    void update(float deltaTime) override;
    void shutdown() override;

    /// Set the world reference for entity iteration
    void setWorld(std::shared_ptr<World> world) { this->world = world; }

    /// Set the GLFW window for input polling
    void setWindow(GLFWwindow* window) { this->window = window; }

    /// Get the GLFW window
    GLFWwindow* getWindow() const { return window; }

    /// Poll input events from GLFW and distribute to entities
    void pollInputEvents();

private:
    std::weak_ptr<World> world;
    GLFWwindow* window = nullptr;

    // Track previous key states to detect state changes
    std::unordered_map<int, int> previousKeyStates;
    std::unordered_map<int, int> previousMouseButtonStates;
    double previousMouseX = 0.0;
    double previousMouseY = 0.0;
};
