cmake_minimum_required(VERSION 3.21)

project(AIECS VERSION 1.0.0 LANGUAGES CXX)

# Set C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find GLM package
find_package(glm CONFIG REQUIRED)

# Source files for Frostbite architecture
set(FROSTBITE_SOURCES
    src/Object.cpp
    src/Module.cpp
    src/EventSystem.cpp
    src/World.cpp
    src/GameEntity.cpp
    src/TransformComponentFB.cpp
    src/CollisionComponentFB.cpp
    src/RenderComponentFB.cpp
)

# Create original executable
add_executable(aiecs_original src/main.cpp)

# Create Frostbite-style executable
add_executable(aiecs_frostbite src/main_frostbite.cpp ${FROSTBITE_SOURCES})

# Create hybrid (Frostbite + SOA) executable
add_executable(aiecs_hybrid src/main_hybrid.cpp ${FROSTBITE_SOURCES})

# Include directories for all
target_include_directories(aiecs_original PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_include_directories(aiecs_frostbite PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_include_directories(aiecs_hybrid PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Link GLM for all
target_link_libraries(aiecs_original PRIVATE glm::glm)
target_link_libraries(aiecs_frostbite PRIVATE glm::glm)
target_link_libraries(aiecs_hybrid PRIVATE glm::glm)

# Set output directory for all
set_target_properties(aiecs_original PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

set_target_properties(aiecs_frostbite PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

set_target_properties(aiecs_hybrid PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
