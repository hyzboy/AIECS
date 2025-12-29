#include "InputComponent.h"
#include <iostream>

InputComponent::InputComponent(const std::string& name)
    : EntityComponent(name) {
}

void InputComponent::onAttach() {
    std::cout << "[InputComponent] Attached to entity" << std::endl;
}

void InputComponent::onDetach() {
    std::cout << "[InputComponent] Detached from entity" << std::endl;
    keyCallbacks.clear();
    mouseButtonCallbacks.clear();
    mouseMoveCallback = nullptr;
    mouseScrollCallback = nullptr;
    keyStates.clear();
    mouseButtonStates.clear();
}

void InputComponent::setKeyCallback(int key, KeyCallback callback) {
    keyCallbacks[key] = callback;
}

void InputComponent::setMouseButtonCallback(int button, MouseButtonCallback callback) {
    mouseButtonCallbacks[button] = callback;
}

void InputComponent::setMouseMoveCallback(MouseMoveCallback callback) {
    mouseMoveCallback = callback;
}

void InputComponent::setMouseScrollCallback(MouseScrollCallback callback) {
    mouseScrollCallback = callback;
}

void InputComponent::processKey(int key, int action, int mods) {
    // Update key state
    keyStates[key] = (action == 1 || action == 2); // GLFW_PRESS or GLFW_REPEAT

    // Call callback if registered
    auto it = keyCallbacks.find(key);
    if (it != keyCallbacks.end() && it->second) {
        it->second(key, action, mods);
    }
}

void InputComponent::processMouseButton(int button, int action, int mods) {
    // Update button state
    mouseButtonStates[button] = (action == 1); // GLFW_PRESS

    // Call callback if registered
    auto it = mouseButtonCallbacks.find(button);
    if (it != mouseButtonCallbacks.end() && it->second) {
        it->second(button, action, mods);
    }
}

void InputComponent::processMouseMove(double xpos, double ypos) {
    lastMouseX = xpos;
    lastMouseY = ypos;

    // Call callback if registered
    if (mouseMoveCallback) {
        mouseMoveCallback(xpos, ypos);
    }
}

void InputComponent::processMouseScroll(double xoffset, double yoffset) {
    // Call callback if registered
    if (mouseScrollCallback) {
        mouseScrollCallback(xoffset, yoffset);
    }
}

bool InputComponent::isKeyPressed(int key) const {
    auto it = keyStates.find(key);
    return it != keyStates.end() && it->second;
}

bool InputComponent::isMouseButtonPressed(int button) const {
    auto it = mouseButtonStates.find(button);
    return it != mouseButtonStates.end() && it->second;
}
