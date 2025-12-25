#include "VAO.h"
#include <iostream>

VAO::VAO() {
    // VAO created on-demand in initialize()
}

VAO::~VAO() {
    cleanup();
}

VAO::VAO(VAO&& other) noexcept
    : vaoID(other.vaoID) {
    other.vaoID = 0;
}

VAO& VAO::operator=(VAO&& other) noexcept {
    if (this != &other) {
        cleanup();
        vaoID = other.vaoID;
        other.vaoID = 0;
    }
    return *this;
}

void VAO::initialize() {
    if (vaoID == 0) {
        glGenVertexArrays(1, &vaoID);
    }
}

void VAO::bind() const {
    glBindVertexArray(vaoID);
}

void VAO::unbind() {
    glBindVertexArray(0);
}

void VAO::setupFloatAttribute(GLuint attributeLocation, GLuint bindingPoint, 
                              GLint componentCount, GLboolean normalized) {
    // ARB_vertex_attrib_binding: Define format
    glVertexAttribFormat(attributeLocation, componentCount, GL_FLOAT, normalized, 0);
    
    // Bind attribute to binding point
    glVertexAttribBinding(attributeLocation, bindingPoint);
    
    // Enable attribute
    glEnableVertexAttribArray(attributeLocation);
}

void VAO::setupIntegerAttribute(GLuint attributeLocation, GLuint bindingPoint, 
                                GLint componentCount) {
    // ARB_vertex_attrib_binding: Define integer format
    glVertexAttribIFormat(attributeLocation, componentCount, GL_UNSIGNED_INT, 0);
    
    // Bind attribute to binding point
    glVertexAttribBinding(attributeLocation, bindingPoint);
    
    // Enable attribute
    glEnableVertexAttribArray(attributeLocation);
}

void VAO::bindVertexBuffer(GLuint bindingPoint, GLuint bufferID, 
                          GLintptr offset, GLsizei stride) {
    // ARB_vertex_attrib_binding: Bind buffer to binding point
    glBindVertexBuffer(bindingPoint, bufferID, offset, stride);
}

void VAO::setBindingDivisor(GLuint bindingPoint, GLuint divisor) {
    // Set divisor for instanced rendering (0 = per-vertex, 1+ = per-instance)
    glVertexBindingDivisor(bindingPoint, divisor);
}

void VAO::cleanup() {
    if (vaoID != 0) {
        glDeleteVertexArrays(1, &vaoID);
        vaoID = 0;
    }
}
