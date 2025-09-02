#include "VBO.h"

// Constructor: creates VBO
VBO::VBO()
{
    glGenBuffers(1, &id);
}

// Destructor: deletes VBO
VBO::~VBO()
{
    glDeleteBuffers(1, &id);
}

// Bind VBO
void VBO::bind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, id);
}

// Unbind VBO
void VBO::unbind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// Upload data to VBO
void VBO::setData(const void* data, GLsizeiptr size, GLenum usage)
{
    glBindBuffer(GL_ARRAY_BUFFER, id);
    glBufferData(GL_ARRAY_BUFFER, size, data, usage);
}

// Get VBO ID
GLuint VBO::getID() const
{
    return id;
}