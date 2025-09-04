#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Shader.h"
#include "Texture2D.h"
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include "Simulation.h"

const int GRID_W = 512;
const int GRID_H = 512;

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


const double SIMULATION_UPDATE_RATE = 50.0;

int main()
{
    GLFWwindow* window = initOpenGLWindow(1024, 1024, "Cellular automata");
    if (!window)
        return -1;

	glfwSwapInterval(1); // Enable vsync

    // Create two 2D textures
	// TODO: Try using Texture2DArray to not bind different textures every frame
	// TODO: Or try to use uniform index to switch between textures in the shader
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
	double textureSwitchCounter = 0.0;
    bool useTextureA = true;

    // Simulation class instance
	Simulation simulation(GRID_W, GRID_H, textureA, textureB);
    simulation.randomize(useTextureA);

    double previousTime = glfwGetTime();
    double uiUpdateTime = previousTime;
    int frameCount = 0;
    int updatesCount = 0;

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        double currentTime = glfwGetTime();
        double deltaTime = currentTime - previousTime;
        if (deltaTime > 0.5)
        {
            deltaTime = 0.0;
            uiUpdateTime = currentTime;
            frameCount = 0;
			updatesCount = 0;
        }
        previousTime = currentTime;

		// Update FPS every 0.5 seconds for stability
        if (currentTime - uiUpdateTime >= 0.5)
        {
            double fps = frameCount / (currentTime - uiUpdateTime);
			double ups = updatesCount / (currentTime - uiUpdateTime);

            uiUpdateTime = currentTime;
            frameCount = 0;
			updatesCount = 0;

            char title[64];
            snprintf(title, sizeof(title), "Cellular automata - FPS: %.1f - UPS: %.1f", fps, ups);
            glfwSetWindowTitle(window, title);
        }
        frameCount++;

        // Switch texture every 1/SIMULATION_UPDATE_RATE seconds
		// Note: If SIMULATION_UPDATE_RATE is going to be dynamic, when increasing it, set textureSwitchCounter to 0.
        {
            textureSwitchCounter += deltaTime;
            int updatesToPerform = static_cast<int>(textureSwitchCounter * SIMULATION_UPDATE_RATE);
            if (updatesToPerform > 0)
            {
                simulation.update(updatesToPerform, useTextureA);
                double updateInterval = 1.0 / SIMULATION_UPDATE_RATE;
                textureSwitchCounter -= updatesToPerform * updateInterval;
                updatesCount += updatesToPerform;
            }
        }

        if (useTextureA)
        {
            textureA.bind(GL_TEXTURE0);
        }
        else
        {
            textureB.bind(GL_TEXTURE0);
        }

        quadRendererShader.use();

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