#include "Texture2D.h"
#include <random>

// Constructor: creates and allocates a 2D texture with custom formats
Texture2D::Texture2D(int width, int height, GLenum internalFormat, GLenum format, GLenum type)
    : width(width), height(height), internalFormat(internalFormat), format(format), type(type)
{
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Allocate storage for the texture
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        internalFormat,
        width,
        height,
        0,
        format,
        type,
        nullptr // No initial data
    );

    // Default parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

// Destructor: deletes the texture
Texture2D::~Texture2D()
{
    glDeleteTextures(1, &textureID);
}

// Bind the texture to a texture unit
void Texture2D::bind(GLenum textureUnit) const
{
    glActiveTexture(textureUnit);
    glBindTexture(GL_TEXTURE_2D, textureID);
}

// Update texture data from user buffer
void Texture2D::setData(const void* data)
{
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexSubImage2D(
        GL_TEXTURE_2D,
        0,
        0, 0,
        width, height,
        format,
        type,
        data
    );
}

// Set texture filtering parameters
void Texture2D::setFilter(GLenum minFilter, GLenum magFilter)
{
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
}

// Set texture wrapping parameters
void Texture2D::setWrap(GLenum wrapS, GLenum wrapT)
{
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
}

// Get OpenGL texture ID
GLuint Texture2D::getID() const
{
    return textureID;
}

// Get texture width
int Texture2D::getWidth() const
{
    return width;
}

// Get texture height
int Texture2D::getHeight() const
{
    return height;
}

GLenum Texture2D::getInternalFormat() const
{
    return internalFormat;
}
