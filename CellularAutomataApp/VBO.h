#pragma once

#include <glad/glad.h>

// VBO class for managing OpenGL Vertex Buffer Objects
class VBO
{
public:
    // Constructor: creates VBO
    VBO();

    // Destructor: deletes VBO
    ~VBO();

    // Bind VBO
    void bind() const;

    // Unbind VBO
    void unbind() const;

    // Upload data to VBO
    void setData(const void* data, GLsizeiptr size, GLenum usage = GL_STATIC_DRAW);

    // Get VBO ID
    GLuint getID() const;

private:
    GLuint id;
};