cmake_minimum_required(VERSION 3.21)

project(AIECS VERSION 1.0.0 LANGUAGES CXX)

# Set C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find GLM package
find_package(glm CONFIG REQUIRED)

add_compile_definitions(GLM_ENABLE_EXPERIMENTAL)

# Find GLEW package
find_package(GLEW REQUIRED)

# Find GLFW package
find_package(glfw3 CONFIG REQUIRED)

set(AIECS_HEADER
include/CollisionComponent.h
include/CollisionDataStorage.h
include/EntityComponent.h
include/EntitySystem.h
include/EventSystem.h
include/GameEntity.h
include/InstanceVBO.h
include/Material.h
include/MobilitySwitcherComponent.h
include/MobilitySwitcherSystem.h
include/Object.h
include/RenderCollector.h
include/RenderComponent.h
include/RenderSystem.h
include/ShaderProgram.h
include/SSBOBuffer.h
include/TransformComponent.h
include/TransformComputeSystem.h
include/TransformDataStorage.h
include/VAO.h
include/VBO.h
include/World.h)


# Source files for hybrid architecture (Frostbite OOP + SOA backend)
set(AIECS_SOURCES
    src/Object.cpp
    src/EntitySystem.cpp
    src/EventSystem.cpp
    src/World.cpp
    src/GameEntity.cpp
    src/TransformComponent.cpp
    src/CollisionComponent.cpp
    src/RenderComponent.cpp
    src/ShaderProgram.cpp
    src/RenderSystem.cpp
    src/RenderCollector.cpp
    src/TransformComputeSystem.cpp
    src/MobilitySwitcherComponent.cpp
    src/MobilitySwitcherSystem.cpp
    src/VAO.cpp
    src/main.cpp
)

# Organize files into Visual Studio filters: put corresponding .h and .cpp into the same group
source_group("Core" REGULAR_EXPRESSION "include/(GameEntity|EntityComponent|EntitySystem|EventSystem|World|Object)\\.h|src/(main|World|Object|GameEntity|EntitySystem|EventSystem)\\.cpp")
source_group("Components" REGULAR_EXPRESSION "include/.*Component.*\\.h|src/.*Component.*\\.cpp")
source_group("Systems" REGULAR_EXPRESSION "include/.*System.*\\.h|src/.*System.*\\.cpp")
source_group("Rendering" REGULAR_EXPRESSION "include/(Render.*|ShaderProgram|VAO|VBO|InstanceVBO|SSBOBuffer|Material|RenderCollector)\\.h|src/(Render.*|ShaderProgram|VAO|VBO|RenderCollector)\\.cpp")
source_group("Data" REGULAR_EXPRESSION "include/.*DataStorage.*\\.h|src/.*DataStorage.*\\.cpp")
source_group("Compute" REGULAR_EXPRESSION "include/TransformComputeSystem\\.h|src/TransformComputeSystem\\.cpp")

# Create main executable (hybrid architecture)
add_executable(aiecs ${AIECS_SOURCES} ${AIECS_HEADER})

# Include directories
target_include_directories(aiecs PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Link GLM, GLEW, GLFW, and OpenGL

target_link_libraries(aiecs PRIVATE glm::glm GLEW::GLEW glfw)

if(WIN32)
    find_package(OpenGL REQUIRED COMPONENTS OpenGL)

    target_link_libraries(aiecs PRIVATE opengl32)
else()
    find_package(OpenGL REQUIRED)

    target_link_libraries(aiecs PRIVATE GL)
endif()

# Set output directory
set_target_properties(aiecs PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
