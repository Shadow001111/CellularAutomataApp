#include "Simulation.h"
#include <glad/glad.h>
#include <math.h>
#include <iostream>

const int WORK_GROUP_W = 8;
const int WORK_GROUP_H = 8;


SimulationSettings::SimulationSettings()
{
    int maxDiameter = MAX_NEIGHBOR_SEARCH_RANGE * 2 + 1;
    int maxTotalCells = maxDiameter * maxDiameter;
    kernel.reserve(maxTotalCells);

    int diameter = neighborSearchRange * 2 + 1;
    int totalCells = diameter * diameter;
	kernel.resize(totalCells, 1.0f);

	kernel[neighborSearchRange + neighborSearchRange * diameter] = 0.0f; // Center cell should not contribute to the sum
}

void SimulationSettings::submitToShader(Shader& shader) const
{
    shader.use();
    shader.setInt("neighborSearchRange", neighborSearchRange);
    shader.setVec2("stableRange", stableRange[0], stableRange[1]);
    shader.setVec2("birthRange", birthRange[0], birthRange[1]);
}

float SimulationSettings::getMaxNeighborSum() const
{
	float maxSum = 0.0f;

	int diameter = neighborSearchRange * 2 + 1;

    for (int r = 0; r < diameter; ++r)
    {
        for (int c = 0; c < diameter; ++c)
        {
            int index = r * diameter + c;
			float value = kernel[index];
            if (value > 0.0f)
            {
				maxSum += value;
            }
        }
	}

    return maxSum;
}

void SimulationSettings::updateKernelSize()
{
    if (neighborSearchRange != previousNeighborSearchRange)
    {
        int diameter = neighborSearchRange * 2 + 1;
        int totalCells = diameter * diameter;
		kernel.resize(totalCells, 1.0f);
    }
	previousNeighborSearchRange = neighborSearchRange;
}

Simulation::Simulation(int gridW, int gridH, Texture2D& texA, Texture2D& texB)
    : gridW(gridW), gridH(gridH), textureA(texA), textureB(texB), gen(rd()), dis(0, 1)
{
    std::vector<Shader::ShaderSource> sources = {
        { GL_COMPUTE_SHADER, "Shaders/automata.comp" }
    };
    computeShader = std::make_unique<Shader>(sources);
    computeShader->use();
    computeShader->setInt("gridWidth", gridW);
    computeShader->setInt("gridHeight", gridH);

    groupsX = ceilf((float)gridW / (float)WORK_GROUP_W);
    groupsY = ceilf((float)gridH / (float)WORK_GROUP_H);

	glGenBuffers(1, &kernelSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, kernelSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, 441 * sizeof(float), nullptr, GL_DYNAMIC_DRAW); // 441 is number of cells with kernel radius 10
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, settings.kernel.size() * sizeof(float), settings.kernel.data());
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, kernelSSBO); // binding = 2
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void Simulation::randomize()
{
    auto& currentTexture = useTextureA ? textureA : textureB;
    std::vector<uint8_t> data(gridW * gridH);
    for (int i = 0; i < gridW * gridH; ++i)
    {
        data[i] = static_cast<uint8_t>(dis(gen));
    }
    currentTexture.setData(data.data());
}

int Simulation::update(double deltaTime)
{
    // Pause
    if (!isRunning)
    {
        simulationUpdateCounter = 0.0;
        return 0;
    }

    // Determine updates to perform
    simulationUpdateCounter += deltaTime;
    int updatesToPerform = static_cast<int>(simulationUpdateCounter * settings.simulationUpdatesRate);
    if (updatesToPerform <= 0)
    {
        return 0;
    }

    simulationUpdateCounter -= (double)updatesToPerform / (double)settings.simulationUpdatesRate;

    // Use compute shader for calculating next world state
    computeShader->use();

    GLuint aID = textureA.getID();
    GLuint bID = textureB.getID();

    for (int i = 0; i < updatesToPerform; i++)
    {
		GLuint currentID = useTextureA ? aID : bID;
		GLuint nextID = useTextureA ? bID : aID;

        // Bind textures to image units
        glBindImageTexture(0, currentID, 0, GL_FALSE, 0, GL_READ_ONLY, GL_R8UI);
        glBindImageTexture(1, nextID, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R8UI);

        // Run compute shader
        glDispatchCompute(groupsX, groupsY, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        // Switch textures
        useTextureA = !useTextureA;
    }
    return updatesToPerform;
}

void Simulation::updateSettingsInShader()
{
	settings.submitToShader(*computeShader);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, kernelSSBO);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, settings.kernel.size() * sizeof(float), settings.kernel.data());
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void Simulation::resetUpdatesCounter()
{
    simulationUpdateCounter = 0.0;
}
