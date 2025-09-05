#include <iostream>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Shader.h"
#include "Texture2D.h"
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"

#include "Simulation.h"
#include "Random.h"
#include "ColorPalette.h"

const int WINDOW_W = 1824;
const int WINDOW_H = 1024;

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

void UI(Simulation& sim, Shader& cellsShader)
{
    ImGui::Begin("Cellular automata");

    SimulationRules& rules = sim.rules;
	SimulationVisuals& visuals = sim.visuals;

    {
        if (ImGui::ListBox("Kernel randomization type", (int*)&rules.kernelRandomizationType, KERNEL_RANDOMIZATION_TYPE_NAMES, IM_ARRAYSIZE(KERNEL_RANDOMIZATION_TYPE_NAMES), 3))
        {
            
		}

        ImGui::Text("Buttons:");

        if (ImGui::Button("Randomize cells"))
        {
            sim.randomize();
        }

        ImGui::SameLine();
        if (ImGui::Button("Reset to default"))
        {
            rules = SimulationRules();
            sim.submitRulesToShader();
            sim.randomize();
        }

        // Randomize rules
        ImGui::SameLine();
        if (ImGui::Button("Randomize rules"))
        {
            // TODO: Use shapes to randomize kernel
            rules.neighborSearchRange = Random::Int(1, SimulationRules::MAX_NEIGHBOR_SEARCH_RANGE);

            rules.updateKernelSize();
            int kernelSize = rules.neighborSearchRange * 2 + 1;

			rules.randomizeKernel();
            float maxNeighborSum = rules.getMaxNeighborSum();

            rules.stableRange[0] = Random::Int(0, (int)maxNeighborSum);
            rules.stableRange[1] = Random::Int(rules.stableRange[0], (int)maxNeighborSum);
            rules.birthRange[0] = Random::Int(0, (int)maxNeighborSum);
            rules.birthRange[1] = Random::Int(rules.birthRange[0], (int)maxNeighborSum);

            sim.randomize();
        }
    }
    ImGui::Dummy({ 0, 20 });

    ImGui::BeginTabBar("Tabs");

    if (ImGui::BeginTabItem("Settings"))
    {
        {
            ImGui::Text("Simulation settings:");

            int previousUpdatesRate = sim.simulationUpdatesRate;
            ImGui::SliderInt("Updates rate", &sim.simulationUpdatesRate, 0, 300);
            if (sim.simulationUpdatesRate != previousUpdatesRate)
            {
                sim.resetUpdatesCounter();
            }

            ImGui::Checkbox("Is running", &sim.isRunning);
        }
        ImGui::Dummy({ 0, 20 });

        {
            ImGui::Text("Simulation rules:");

            ImGui::SliderInt("Neighbor search range", &rules.neighborSearchRange, 1, SimulationRules::MAX_NEIGHBOR_SEARCH_RANGE);
            rules.updateKernelSize();
            float maxNeighborSum = rules.getMaxNeighborSum();

            int stableRange[2] = { (int)rules.stableRange[0], (int)rules.stableRange[1] };
            ImGui::SliderInt("Stable range", &stableRange[0], 0, (int)rules.stableRange[1]);
            rules.stableRange[0] = (float)stableRange[0];

            ImGui::SliderInt("##0", &stableRange[1], (int)rules.stableRange[0], (int)maxNeighborSum);
            rules.stableRange[1] = (float)stableRange[1];

            int birthRange[2] = { (int)rules.birthRange[0], (int)rules.birthRange[1] };
            ImGui::SliderInt("Birth range", &birthRange[0], 0, (int)rules.birthRange[1]);
            rules.birthRange[0] = (float)birthRange[0];

            ImGui::SliderInt("##1", &birthRange[1], (int)rules.birthRange[0], (int)maxNeighborSum);
            rules.birthRange[1] = (float)birthRange[1];
        }
        ImGui::Dummy({ 0, 20 });

        {
            ImGui::Text("Kernel:");

            int kernelSize = rules.neighborSearchRange * 2 + 1;

            // Calculate cell size based on window size
            const float maxDisplaySize = WINDOW_H * 0.5f;

            ImVec2 avail = ImGui::GetContentRegionAvail();
            if (avail.x > 0 && avail.y > 0)
            {
                avail.x = std::min(avail.x, maxDisplaySize);
                avail.y = std::min(avail.y, maxDisplaySize);

                float cellWidth = avail.x / kernelSize;
                float cellHeight = avail.y / kernelSize;

                ImDrawList* drawList = ImGui::GetWindowDrawList();
                ImVec2 origin = ImGui::GetCursorScreenPos();

                for (int r = 0; r < kernelSize; ++r)
                {
                    for (int c = 0; c < kernelSize; ++c)
                    {
                        float value = rules.kernel[r * kernelSize + c];

                        ImVec4 col;
                        if (value < 0.0f)
                        {
                            // Red for negative values
                            float power = value / SimulationRules::KERNEL_MIN_VALUE;
                            col = ImVec4(1.0f, 1.0f - power, 1.0f - power, 1.0f);
                        }
                        else
                        {
                            // Green for positive values
                            float power = value / SimulationRules::KERNEL_MAX_VALUE;
                            col = ImVec4(1.0f - power, 1.0f, 1.0f - power, 1.0f);
                        }

                        ImVec2 p0(origin.x + c * cellWidth, origin.y + r * cellHeight);
                        ImVec2 p1(p0.x + cellWidth, p0.y + cellHeight);

                        drawList->AddRectFilled(p0, p1, ImColor(col));

                        // Optional: draw the value text
                        char buf[16];
                        snprintf(buf, sizeof(buf), "%.1f", value);
                        drawList->AddText(ImVec2(p0.x + 2, p0.y + 2), IM_COL32(0, 0, 0, 255), buf);
                    }
                }

                ImGui::Dummy(avail); // reserve space for the grid
            }
        }
        ImGui::Dummy({ 0, 20 });

        ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("Visuals"))
    {
        if (ImGui::Button("Generate monochromatic colors"))
        {
            ColorPalette::generateMonochromaticOKLab(visuals.aliveColor, visuals.deadColor);
			sim.submitVisualsToShader(cellsShader);
        }

        ImGui::SameLine();
        if (ImGui::Button("Generate analogous colors"))
        {
            ColorPalette::generateAnalogousOKLab(visuals.aliveColor, visuals.deadColor);
            sim.submitVisualsToShader(cellsShader);
        }

        ImGui::SameLine();
        if (ImGui::Button("Generate complementary colors"))
        {
            ColorPalette::generateComplementaryOKLab(visuals.aliveColor, visuals.deadColor);
            sim.submitVisualsToShader(cellsShader);
        }

		ImGui::SameLine();
        if (ImGui::Button("Choose random"))
        {
            ColorPalette::generateRandomOKLab(visuals.aliveColor, visuals.deadColor);
            sim.submitVisualsToShader(cellsShader);
        }

        ImGui::EndTabItem();
	}
	
    ImGui::EndTabBar();

	ImGui::End();

	// Sumbit values to compute shader
    // TODO: update only when settings got changed
	sim.submitRulesToShader();

	// TODO: Add ability to save and load rules
	// TODO: Add ability to choose kernel generation method (only positive ints, only 0 or 1, only positives, any)
	// TODO: Add ability to change kernel size max and min values
    // TODO; Add ability to use keyboard for changing settings
}

int main()
{
    GLFWwindow* window = initOpenGLWindow(WINDOW_W, WINDOW_H, "Cellular automata");
    if (!window)
        return -1;

	glfwSwapInterval(1); // Enable vsync

    // Create two 2D textures
    Texture2D textureA(GRID_W, GRID_H, GL_R8UI, GL_RED_INTEGER, GL_UNSIGNED_BYTE);
    Texture2D textureB(GRID_W, GRID_H, GL_R8UI, GL_RED_INTEGER, GL_UNSIGNED_BYTE);

    // Load shaders using Shader class
    std::vector<Shader::ShaderSource> cellsRendererShaderSources = {
        { GL_VERTEX_SHADER,   "Shaders/cells.vert" },
        { GL_FRAGMENT_SHADER, "Shaders/cells.frag" }
    };
    Shader cellsRendererShader(cellsRendererShaderSources);

    // Fullscreen quad vertices and indices
	float windowAspectRatio = static_cast<float>(WINDOW_W) / static_cast<float>(WINDOW_H);
	float gridAspectRatio = static_cast<float>(GRID_W) / static_cast<float>(GRID_H);

    float vertices[] = {
        // positions   // texcoords
         0.0f, -windowAspectRatio * 0.5f / gridAspectRatio,  0.0f, 0.0f,
         1.0f, -windowAspectRatio * 0.5f / gridAspectRatio,  1.0f, 0.0f,
         1.0f,  windowAspectRatio * 0.5f / gridAspectRatio,  1.0f, 1.0f,
         0.0f,  windowAspectRatio * 0.5f / gridAspectRatio,  0.0f, 1.0f
    };
    unsigned int indices[] = { 0, 1, 2, 2, 3, 0 };

    VAO vao;
    VBO vbo;
    EBO ebo;

    // Create and configure quad buffers
    createQuadBuffers(vao, vbo, ebo, vertices, sizeof(vertices), indices, sizeof(indices));

    // Simulation class instance
	Simulation simulation(GRID_W, GRID_H, textureA, textureB);
	ColorPalette::generateRandomHSV(simulation.visuals.aliveColor, simulation.visuals.deadColor);
	simulation.submitVisualsToShader(cellsRendererShader);

    double previousTime = glfwGetTime();
    double uiUpdateTime = previousTime;
    int frameCount = 0;
    int updatesCount = 0;

	// imgui initialization
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");

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
        updatesCount += simulation.update(deltaTime);

        // Clear screen
        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        if (simulation.useTextureA)
        {
            textureA.bind(GL_TEXTURE0);
        }
        else
        {
            textureB.bind(GL_TEXTURE0);
        }

        cellsRendererShader.use();

        vao.bind();
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // imgui new frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        //
        UI(simulation, cellsRendererShader);

        // imgui render
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Очистка ImGui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // Destructors will delete the textures and buffers
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}