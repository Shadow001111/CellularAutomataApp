#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Shader.h"
#include "Texture2D.h"
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include "Simulation.h"

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


const double SIMULATION_UPDATE_RATE = 120.0;

int main()
{
    GLFWwindow* window = initOpenGLWindow(1920, 1080, "OpenGL 4.6 Window");
    if (!window)
        return -1;

    // Create two 2D textures
    Texture2D textureA(GRID_W, GRID_H, GL_R8UI, GL_RED_INTEGER, GL_UNSIGNED_BYTE);
    Texture2D textureB(GRID_W, GRID_H, GL_R8UI, GL_RED_INTEGER, GL_UNSIGNED_BYTE);

    // Load shaders using Shader class
    std::vector<Shader::ShaderSource> quadRendererShaderSources = {
        { GL_VERTEX_SHADER,   "Shaders/texture.vert" },
        { GL_FRAGMENT_SHADER, "Shaders/texture.frag" }
    };
    Shader quadRendererShader(quadRendererShaderSources);

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

    // Timing variables for texture switching
    double lastSwitchTime = glfwGetTime();
    bool useTextureA = true;

    // Simulation class instance
	Simulation simulation(GRID_W, GRID_H, textureA, textureB);
    simulation.randomize(useTextureA);

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        quadRendererShader.use();

        // Switch texture every 1/SIMULATION_UPDATE_RATE seconds
        double currentTime = glfwGetTime();
        if (currentTime - lastSwitchTime >= 1.0 / SIMULATION_UPDATE_RATE)
        {
            simulation.update(useTextureA);
            useTextureA = !useTextureA;
            lastSwitchTime = currentTime;
        }

        if (useTextureA)
        {
            textureA.bind(GL_TEXTURE0);
        }
        else
        {
            textureB.bind(GL_TEXTURE0);
        }
        quadRendererShader.setInt("uTexture", 0);

        vao.bind();
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Destructors will delete the textures and buffers
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}