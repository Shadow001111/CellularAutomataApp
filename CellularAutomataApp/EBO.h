#pragma once

#include <glad/glad.h>

// EBO class for managing OpenGL Element Buffer Objects
class EBO
{
public:
    // Constructor: creates EBO
    EBO();

    // Destructor: deletes EBO
    ~EBO();

    // Bind EBO
    void bind() const;

    // Unbind EBO
    void unbind() const;

    // Upload data to EBO
    void setData(const void* data, GLsizeiptr size, GLenum usage = GL_STATIC_DRAW);

    // Get EBO ID
    GLuint getID() const;

private:
    GLuint id;
};