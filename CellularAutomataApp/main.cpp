#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Shader.h"
#include <random>

const int GRID_W = 128;
const int GRID_H = 128;

GLFWwindow* initOpenGLWindow(int width, int height, const char* title)
{
    // Initialize GLFW
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return nullptr;
    }

    // Set OpenGL version to 4.6 Core
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef _DEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif

    // Create window
    GLFWwindow* window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!window)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return nullptr;
    }

    // Set context
    glfwMakeContextCurrent(window);

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return nullptr;
    }

    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
    return window;
}

// Update texture data with random 0 or 1 values
void updateTextureRandom(GLuint textureID, int width, int height)
{
    // Create random generator
    static std::mt19937 rng(std::random_device{}());
    static std::uniform_int_distribution<int> dist(0, 1);

    // Allocate buffer for texture data
    std::vector<unsigned char> data(width * height);

    // Fill buffer with random 0 or 1
    for (int i = 0; i < width * height; ++i)
        data[i] = static_cast<unsigned char>(dist(rng));

    // Update texture data
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexSubImage2D(
        GL_TEXTURE_2D,
        0,
        0, 0,
        width, height,
        GL_RED_INTEGER,
        GL_UNSIGNED_BYTE,
        data.data()
    );
}

int main()
{
    GLFWwindow* window = initOpenGLWindow(1920, 1080, "OpenGL 4.6 Window");
    if (!window)
        return -1;

    // Create 2D texture
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Allocate storage for the texture
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_R8UI,           // Internal format: 8-bit unsigned integer, use only 0 or 1
        GRID_W,
        GRID_H,
        0,
        GL_RED_INTEGER,    // Format for integer textures
        GL_UNSIGNED_BYTE,  // Type
        nullptr            // No initial data
    );

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Load shaders using Shader class
    std::vector<Shader::ShaderSource> sources = {
        { GL_VERTEX_SHADER,   "Shaders/texture.vert" },
        { GL_FRAGMENT_SHADER, "Shaders/texture.frag" }
    };
    Shader shader(sources);

    // Fullscreen quad vertices and indices
    float vertices[] = {
        // positions   // texcoords
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f,
        -1.0f,  1.0f,  0.0f, 1.0f
    };
    unsigned int indices[] = { 0, 1, 2, 2, 3, 0 };

    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // TexCoord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    // Initial texture update
    updateTextureRandom(textureID, GRID_W, GRID_H);

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        updateTextureRandom(textureID, GRID_W, GRID_H);

        glClearColor(0.9f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shader.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureID);
        shader.setInt("uTexture", 0);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteTextures(1, &textureID);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}