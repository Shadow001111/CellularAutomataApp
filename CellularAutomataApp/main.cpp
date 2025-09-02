#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Shader.h"
#include "Texture2D.h"
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include <random> // For random number generation

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

void createQuadBuffers(VAO& vao, VBO& vbo, EBO& ebo, const float* vertices, size_t verticesSize, const unsigned int* indices, size_t indicesSize)
{
    // Bind VAO
    vao.bind();

    // Bind and fill VBO
    vbo.bind();
    vbo.setData(vertices, verticesSize);

    // Bind and fill EBO
    ebo.bind();
    ebo.setData(indices, indicesSize);

    // Configure vertex attributes
    // Position attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // TexCoord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Unbind VAO
    vao.unbind();
}

int main()
{
    GLFWwindow* window = initOpenGLWindow(1920, 1080, "OpenGL 4.6 Window");
    if (!window)
        return -1;

    // Create two 2D textures using Texture2D class
    Texture2D textureA(GRID_W, GRID_H, GL_R8UI, GL_RED_INTEGER, GL_UNSIGNED_BYTE);
    Texture2D textureB(GRID_W, GRID_H, GL_R8UI, GL_RED_INTEGER, GL_UNSIGNED_BYTE);

    // Fill both textures with random values between 0 and 1
    // Комментарий: Fill texture data arrays with random 0 or 1 values
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 1);

    std::vector<uint8_t> dataA(GRID_W * GRID_H);
    std::vector<uint8_t> dataB(GRID_W * GRID_H);
    for (int i = 0; i < GRID_W * GRID_H; ++i)
    {
        dataA[i] = static_cast<uint8_t>(dis(gen));
        dataB[i] = static_cast<uint8_t>(dis(gen));
    }

    // Комментарий: Upload random data to textures
    textureA.setData(dataA.data());
    textureB.setData(dataB.data());

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

    VAO vao;
    VBO vbo;
    EBO ebo;

    // Create and configure quad buffers
    createQuadBuffers(vao, vbo, ebo, vertices, sizeof(vertices), indices, sizeof(indices));

    int frameCount = 0; // Frames counter

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shader.use();

        // Alternate between textureA and textureB every frame
        if ((frameCount % 2) == 0)
        {
            textureA.bind(GL_TEXTURE0);
        }
        else
        {
            textureB.bind(GL_TEXTURE0);
        }
        shader.setInt("uTexture", 0);

        vao.bind();
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();

        frameCount++;
    }

    // Destructors will delete the textures and buffers
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}