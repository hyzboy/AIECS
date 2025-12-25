cmake_minimum_required(VERSION 3.21)

project(AIECS VERSION 1.0.0 LANGUAGES CXX)

# Set C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find GLM package
find_package(glm CONFIG REQUIRED)

# Find GLEW package
find_package(GLEW REQUIRED)

# Find GLFW package
find_package(glfw3 CONFIG REQUIRED)

# Source files for hybrid architecture (Frostbite OOP + SOA backend)
set(AIECS_SOURCES
    src/Object.cpp
    src/Module.cpp
    src/EventSystem.cpp
    src/World.cpp
    src/GameEntity.cpp
    src/TransformComponent.cpp
    src/CollisionComponent.cpp
    src/RenderComponent.cpp
    src/main.cpp
)

# Create main executable (hybrid architecture)
add_executable(aiecs ${AIECS_SOURCES})

# Include directories
target_include_directories(aiecs PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Link GLM, GLEW, and GLFW
target_link_libraries(aiecs PRIVATE glm::glm GLEW::GLEW glfw)

# Set output directory
set_target_properties(aiecs PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
