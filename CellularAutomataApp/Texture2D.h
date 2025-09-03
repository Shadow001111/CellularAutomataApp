#pragma once
#include <glad/glad.h>
#include <vector>

// Texture2D class for managing 2D OpenGL textures
class Texture2D
{
public:
    Texture2D(
        int width,
        int height,
        GLenum internalFormat = GL_R8UI,
        GLenum format = GL_RED_INTEGER,
        GLenum type = GL_UNSIGNED_BYTE
    );
    ~Texture2D();
    Texture2D(const Texture2D&) = delete;
    Texture2D& operator=(const Texture2D&) = delete;
    Texture2D(Texture2D&& other) noexcept;
    Texture2D& operator=(Texture2D&& other) noexcept;

    void bind(GLenum textureUnit = GL_TEXTURE0) const;
    void setData(const void* data);
    void setFilter(GLenum minFilter, GLenum magFilter);
    void setWrap(GLenum wrapS, GLenum wrapT);
    GLuint getID() const;
    int getWidth() const;
    int getHeight() const;
    GLenum getInternalFormat() const;
private:
    GLuint textureID;
    int width;
    int height;
    GLenum internalFormat;
    GLenum format;
    GLenum type;
};