#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

int main() {
    std::cout << "AIECS - C++20 Project with GLM" << std::endl;
    std::cout << "=============================" << std::endl;
    
    // Create a simple GLM vector
    glm::vec3 position(1.0f, 2.0f, 3.0f);
    std::cout << "Position vector: (" 
              << position.x << ", " 
              << position.y << ", " 
              << position.z << ")" << std::endl;
    
    // Create a simple GLM matrix
    glm::mat4 identity = glm::mat4(1.0f);
    std::cout << "Identity matrix created successfully!" << std::endl;
    
    // Perform a simple transformation
    glm::vec3 translation(5.0f, 0.0f, 0.0f);
    glm::mat4 transform = glm::translate(identity, translation);
    std::cout << "Translation matrix created successfully!" << std::endl;
    
    std::cout << "\nGLM library is working correctly!" << std::endl;
    
    return 0;
}
