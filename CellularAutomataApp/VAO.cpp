#include "VAO.h"

// Constructor: creates VAO
VAO::VAO()
{
    glGenVertexArrays(1, &id);
}

// Destructor: deletes VAO
VAO::~VAO()
{
    glDeleteVertexArrays(1, &id);
}

// Bind VAO
void VAO::bind() const
{
    glBindVertexArray(id);
}

// Unbind VAO
void VAO::unbind() const
{
    glBindVertexArray(0);
}

// Get VAO ID
GLuint VAO::getID() const
{
    return id;
}