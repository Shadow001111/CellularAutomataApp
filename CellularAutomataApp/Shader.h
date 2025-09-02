#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <glad/glad.h>

class Shader
{
public:
    // Structure describing a shader
    struct ShaderSource {
        GLenum type;
        std::string path;
    };

    // Constructor: takes a list of shaders (type + file path)
    Shader(const std::vector<ShaderSource>& sources);
    ~Shader();

    void use() const;

    // Uniform setters
    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setVec3(const std::string& name, float x, float y, float z) const;
    void setMat4(const std::string& name, const float* mat) const;

    GLuint getID() const { return ID; }

private:
    GLuint ID;
    mutable std::unordered_map<std::string, GLint> uniformLocationCache;

    // Returns cached uniform location or queries and caches it
    GLint getUniformLocation(const std::string& name) const;

    // Loads shader source code from file
    std::string loadShaderSource(const std::string& filePath) const;
    // Compiles a shader
    GLuint compileShader(GLenum type, const std::string& source) const;
    // Checks for compilation/linking errors
    void checkCompileErrors(GLuint shader, const std::string& type) const;
};