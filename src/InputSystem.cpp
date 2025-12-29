#include "InputSystem.h"
#include "InputComponent.h"
#include "GameEntity.h"
#include <GLFW/glfw3.h>
#include <iostream>

InputSystem::InputSystem(const std::string& name)
    : EntitySystem(name) {
}

InputSystem::~InputSystem() {
    shutdown();
}

void InputSystem::initialize() {
    std::cout << "[InputSystem] Initializing input system..." << std::endl;
    initialized = true;
}

void InputSystem::update(float deltaTime) {
    if (!initialized || !window) return;

    // Poll input events and distribute to entities with InputComponent
    pollInputEvents();
}

void InputSystem::pollInputEvents() {
    auto worldPtr = world.lock();
    if (!worldPtr) return;

    // Get current mouse position
    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);

    // Check if mouse moved
    bool mouseMoved = (mouseX != previousMouseX || mouseY != previousMouseY);
    if (mouseMoved) {
        previousMouseX = mouseX;
        previousMouseY = mouseY;
    }

    // Iterate through all entities in the world
    // Note: This simple iteration approach is consistent with other systems (e.g., MobilitySwitcherSystem).
    // In a more optimized implementation, you could maintain a separate collection of entities with InputComponent.
    const auto& objects = worldPtr->getObjects();
    for (const auto& obj : objects) {
        // Try to cast to GameEntity
        auto entity = std::dynamic_pointer_cast<GameEntity>(obj);
        if (!entity) continue;

        // Check if entity has InputComponent
        auto inputComponent = entity->getComponent<InputComponent>();
        if (!inputComponent) continue;

        // Process mouse movement if it moved
        if (mouseMoved) {
            inputComponent->processMouseMove(mouseX, mouseY);
        }

        // Check for key state changes
        // We check commonly used keys - in a real implementation, 
        // you might want to track which keys are registered in callbacks
        static const int keysToCheck[] = {
            GLFW_KEY_W, GLFW_KEY_A, GLFW_KEY_S, GLFW_KEY_D,
            GLFW_KEY_SPACE, GLFW_KEY_ESCAPE, GLFW_KEY_ENTER,
            GLFW_KEY_LEFT, GLFW_KEY_RIGHT, GLFW_KEY_UP, GLFW_KEY_DOWN,
            GLFW_KEY_LEFT_SHIFT, GLFW_KEY_LEFT_CONTROL,
            GLFW_KEY_Q, GLFW_KEY_E, GLFW_KEY_R, GLFW_KEY_F,
            GLFW_KEY_1, GLFW_KEY_2, GLFW_KEY_3, GLFW_KEY_4, GLFW_KEY_5
        };

        for (int key : keysToCheck) {
            int state = glfwGetKey(window, key);
            int prevState = previousKeyStates[key];
            
            if (state != prevState) {
                previousKeyStates[key] = state;
                inputComponent->processKey(key, state, 0);
            }
        }

        // Check for mouse button state changes
        static const int buttonsToCheck[] = {
            GLFW_MOUSE_BUTTON_LEFT,
            GLFW_MOUSE_BUTTON_RIGHT,
            GLFW_MOUSE_BUTTON_MIDDLE
        };

        for (int button : buttonsToCheck) {
            int state = glfwGetMouseButton(window, button);
            int prevState = previousMouseButtonStates[button];
            
            if (state != prevState) {
                previousMouseButtonStates[button] = state;
                inputComponent->processMouseButton(button, state, 0);
            }
        }
    }
}

void InputSystem::shutdown() {
    std::cout << "[InputSystem] Shutting down..." << std::endl;
    initialized = false;
    window = nullptr;
    previousKeyStates.clear();
    previousMouseButtonStates.clear();
}
