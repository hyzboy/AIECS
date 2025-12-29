#pragma once

#include "EntityComponent.h"
#include <functional>
#include <unordered_map>

/// Component that enables input handling for entities
/// Entities with this component can respond to keyboard and mouse inputs
class InputComponent : public EntityComponent {
public:
    /// Callback types for input events
    using KeyCallback = std::function<void(int key, int action, int mods)>;
    using MouseButtonCallback = std::function<void(int button, int action, int mods)>;
    using MouseMoveCallback = std::function<void(double xpos, double ypos)>;
    using MouseScrollCallback = std::function<void(double xoffset, double yoffset)>;

    explicit InputComponent(const std::string& name = "InputComponent");
    ~InputComponent() override = default;

    /// Register a callback for key events
    /// @param key - GLFW key code (e.g., GLFW_KEY_W)
    /// @param callback - Function to call when key state changes
    void setKeyCallback(int key, KeyCallback callback);

    /// Register a callback for mouse button events
    /// @param button - GLFW mouse button (e.g., GLFW_MOUSE_BUTTON_LEFT)
    /// @param callback - Function to call when button state changes
    void setMouseButtonCallback(int button, MouseButtonCallback callback);

    /// Register a callback for mouse movement
    void setMouseMoveCallback(MouseMoveCallback callback);

    /// Register a callback for mouse scroll
    void setMouseScrollCallback(MouseScrollCallback callback);

    /// Process key input (called by InputSystem)
    void processKey(int key, int action, int mods);

    /// Process mouse button input (called by InputSystem)
    void processMouseButton(int button, int action, int mods);

    /// Process mouse movement (called by InputSystem)
    void processMouseMove(double xpos, double ypos);

    /// Process mouse scroll (called by InputSystem)
    void processMouseScroll(double xoffset, double yoffset);

    /// Check if a specific key is currently pressed
    bool isKeyPressed(int key) const;

    /// Check if a specific mouse button is currently pressed
    bool isMouseButtonPressed(int button) const;

    /// Get last mouse position
    void getMousePosition(double& xpos, double& ypos) const {
        xpos = lastMouseX;
        ypos = lastMouseY;
    }

    void onAttach() override;
    void onDetach() override;

private:
    // Key callbacks
    std::unordered_map<int, KeyCallback> keyCallbacks;
    
    // Mouse button callbacks
    std::unordered_map<int, MouseButtonCallback> mouseButtonCallbacks;
    
    // Mouse movement callback
    MouseMoveCallback mouseMoveCallback;
    
    // Mouse scroll callback
    MouseScrollCallback mouseScrollCallback;

    // Input state tracking
    std::unordered_map<int, bool> keyStates;
    std::unordered_map<int, bool> mouseButtonStates;
    
    // Last mouse position
    double lastMouseX = 0.0;
    double lastMouseY = 0.0;
};
