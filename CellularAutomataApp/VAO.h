#pragma once

#include <glad/glad.h>

// VAO class for managing OpenGL Vertex Array Objects
class VAO
{
public:
    // Constructor: creates VAO
    VAO();

    // Destructor: deletes VAO
    ~VAO();

    // Bind VAO
    void bind() const;

    // Unbind VAO
    void unbind() const;

    // Get VAO ID
    GLuint getID() const;

private:
    GLuint id;
};