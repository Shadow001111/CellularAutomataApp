#include "Texture2D.h"
#include <random>

Texture2D::Texture2D(int width, int height, GLenum internalFormat, GLenum format, GLenum type)
    : width(width), height(height), internalFormat(internalFormat), format(format), type(type)
{
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        internalFormat,
        width,
        height,
        0,
        format,
        type,
        nullptr
    );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

Texture2D::~Texture2D()
{
    glDeleteTextures(1, &textureID);
}

Texture2D::Texture2D(Texture2D&& other) noexcept
    : textureID(other.textureID),
    width(other.width),
    height(other.height),
    internalFormat(other.internalFormat),
    format(other.format),
    type(other.type)
{
    other.textureID = 0;
}

Texture2D& Texture2D::operator=(Texture2D&& other) noexcept
{
    if (this != &other)
    {
        glDeleteTextures(1, &textureID);

        width = other.width;
        height = other.height;
        internalFormat = other.internalFormat;
        format = other.format;
        type = other.type;
        textureID = other.textureID;

        other.textureID = 0;
    }
    return *this;
}

void Texture2D::bind(GLenum textureUnit) const
{
    glActiveTexture(textureUnit);
    glBindTexture(GL_TEXTURE_2D, textureID);
}

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

void Texture2D::setFilter(GLenum minFilter, GLenum magFilter)
{
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
}

void Texture2D::setWrap(GLenum wrapS, GLenum wrapT)
{
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
}

GLuint Texture2D::getID() const
{
    return textureID;
}

int Texture2D::getWidth() const
{
    return width;
}

int Texture2D::getHeight() const
{
    return height;
}

GLenum Texture2D::getInternalFormat() const
{
    return internalFormat;
}
