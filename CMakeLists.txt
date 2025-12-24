cmake_minimum_required(VERSION 3.21)

project(AIECS VERSION 1.0.0 LANGUAGES CXX)

# Set C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find GLM package
find_package(glm CONFIG REQUIRED)

# Create executable
add_executable(aiecs src/main.cpp)

# Link GLM
target_link_libraries(aiecs PRIVATE glm::glm)

# Set output directory
set_target_properties(aiecs PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
