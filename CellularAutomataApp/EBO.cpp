#include "EBO.h"

// Constructor: creates EBO
EBO::EBO()
{
    glGenBuffers(1, &id);
}

// Destructor: deletes EBO
EBO::~EBO()
{
    glDeleteBuffers(1, &id);
}

// Bind EBO
void EBO::bind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
}

// Unbind EBO
void EBO::unbind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

// Upload data to EBO
void EBO::setData(const void* data, GLsizeiptr size, GLenum usage)
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, usage);
}

// Get EBO ID
GLuint EBO::getID() const
{
    return id;
}