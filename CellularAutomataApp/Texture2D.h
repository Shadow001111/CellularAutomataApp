#pragma once
#include <glad/glad.h>
#include <vector>

// Texture2D class for managing 2D OpenGL textures with configurable parameters
class Texture2D
{
public:
    // Constructor: creates and allocates a 2D texture with custom formats
    Texture2D(
        int width,
        int height,
        GLenum internalFormat = GL_R8UI,      // Internal format
        GLenum format = GL_RED_INTEGER,       // Data format
        GLenum type = GL_UNSIGNED_BYTE        // Data type
    );

    // Destructor: deletes the texture
    ~Texture2D();

    // Bind the texture to a texture unit
    void bind(GLenum textureUnit = GL_TEXTURE0) const;

    // Update texture data from user buffer
    void update(const void* data);

    // Set texture filtering and wrapping parameters
    void setFilter(GLenum minFilter, GLenum magFilter);
    void setWrap(GLenum wrapS, GLenum wrapT);

    // Get OpenGL texture ID
    GLuint getID() const;

    // Get texture width
    int getWidth() const;

    // Get texture height
    int getHeight() const;

private:
    GLuint textureID;
    int width;
    int height;
    GLenum internalFormat;
    GLenum format;
    GLenum type;
};