#include "Simulation.h"
#include <glad/glad.h>
#include <math.h>
#include <iostream>
#include "Random.h"

const int WORK_GROUP_W = 8;
const int WORK_GROUP_H = 8;


SimulationRules::SimulationRules()
{
    int maxDiameter = MAX_NEIGHBOR_SEARCH_RANGE * 2 + 1;
    int maxTotalCells = maxDiameter * maxDiameter;
    kernel.reserve(maxTotalCells);

    int diameter = neighborSearchRange * 2 + 1;
    int totalCells = diameter * diameter;
	kernel.resize(totalCells, 1.0f);

	kernel[neighborSearchRange + neighborSearchRange * diameter] = 0.0f; // Center cell should not contribute to the sum
}

void SimulationRules::submitToShader(Shader& shader) const
{
    shader.use();
    shader.setInt("neighborSearchRange", neighborSearchRange);
    shader.setUvec2("stableRange", stableRange[0], stableRange[1]);
    shader.setUvec2("birthRange", birthRange[0], birthRange[1]);
}

float SimulationRules::getMaxNeighborSum() const
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

void SimulationRules::updateKernelSize()
{
    if (neighborSearchRange != previousNeighborSearchRange)
    {
        int diameter = neighborSearchRange * 2 + 1;
        int totalCells = diameter * diameter;
		kernel.resize(totalCells, 1.0f);
    }
	previousNeighborSearchRange = neighborSearchRange;
}

void SimulationRules::randomizeKernel()
{
    if (kernelRandomizationType == KernelGenerationType::RandomAllValues)
    {
        const float leftBorder = 0.2f;
        const float rightBorder = 0.8f;

        for (int i = 0; i < kernel.size(); ++i)
        {
            float value = Random::Float(0.0f, 1.0f);

            if (value < leftBorder)
            {
                // Scale to [SimulationRules::KERNEL_MIN_VALUE, 1]
                value = SimulationRules::KERNEL_MIN_VALUE + value / leftBorder * (1.0f - SimulationRules::KERNEL_MIN_VALUE);
            }
            else if (value <= rightBorder)
            {
                value = 1.0f;
            }
            else
            {
                // Scale to [1, SimulationRules::KERNEL_MAX_VALUE]
                value = 1.0f + (value - rightBorder) / (1.0f - rightBorder) * (SimulationRules::KERNEL_MAX_VALUE - 1.0f);
            }

            kernel[i] = value;
        }
    }
    else if (kernelRandomizationType == KernelGenerationType::RandomOnlyPositives)
    {
        for (int i = 0; i < kernel.size(); ++i)
        {
            float value = Random::Float(1.0f, (float)SimulationRules::KERNEL_MAX_VALUE);
            kernel[i] = value;
        }
    }
    else if (kernelRandomizationType == KernelGenerationType::RandomOnlyZerosAndOnes)
    {
        for (int i = 0; i < kernel.size(); ++i)
        {
            float value = static_cast<float>(Random::Int(0, 1));
            kernel[i] = value;
        }
	}
    else if (kernelRandomizationType == KernelGenerationType::VonNeumann)
    {
        int diameter = neighborSearchRange * 2 + 1;
        int center = neighborSearchRange;
        for (int x = 0; x < diameter; ++x)
        {
            for (int y = 0; y < diameter; ++y)
            {
                int index = x * diameter + y;
                if (abs(x - center) + abs(y - center) <= neighborSearchRange)
                {
                    kernel[index] = 1.0f;
                }
                else
                {
                    kernel[index] = 0.0f;
                }
            }
        }
	}
    else if (kernelRandomizationType == KernelGenerationType::FilledCircle)
    {
        int diameter = neighborSearchRange * 2 + 1;
        int center = neighborSearchRange;
        float radius = neighborSearchRange;
        float radiusSquared = radius * radius;
        for (int x = 0; x < diameter; ++x)
        {
            for (int y = 0; y < diameter; ++y)
            {
                int index = x * diameter + y;
                float dx = static_cast<float>(x - center);
                float dy = static_cast<float>(y - center);
                float distanceSquared = dx * dx + dy * dy;
                if (distanceSquared <= radiusSquared)
                {
                    kernel[index] = neighborSearchRange - sqrtf(distanceSquared);
                }
                else
                {
                    kernel[index] = 0.0f;
                }
            }
		}
	}
    else if (kernelRandomizationType == KernelGenerationType::FilledCircleWithNegatives)
    {
        int diameter = neighborSearchRange * 2 + 1;
        int center = neighborSearchRange;
        float radius = neighborSearchRange;
        float radiusSquared = radius * radius;
        for (int x = 0; x < diameter; ++x)
        {
            for (int y = 0; y < diameter; ++y)
            {
                int index = x * diameter + y;
                float dx = static_cast<float>(x - center);
                float dy = static_cast<float>(y - center);
                float distanceSquared = dx * dx + dy * dy;
                if (distanceSquared <= radiusSquared)
                {
                    kernel[index] = neighborSearchRange - sqrtf(distanceSquared);
                }
                else
                {
                    kernel[index] = neighborSearchRange - sqrtf(distanceSquared);
                }
            }
        }
	}
    else if (kernelRandomizationType == KernelGenerationType::Checkerboard)
    {
        int diameter = neighborSearchRange * 2 + 1;
        for (int x = 0; x < diameter; ++x)
        {
            for (int y = 0; y < diameter; ++y)
            {
                int index = x * diameter + y;
                if ((x + y) % 2 == 0)
                {
                    kernel[index] = 1.0f;
                }
                else
                {
                    kernel[index] = 0.0f;
                }
            }
        }
	}
    else if (kernelRandomizationType == KernelGenerationType::CheckerboardWithNegatives)
    {
        int diameter = neighborSearchRange * 2 + 1;
        for (int x = 0; x < diameter; ++x)
        {
            for (int y = 0; y < diameter; ++y)
            {
                int index = x * diameter + y;
                if ((x + y) % 2 == 0)
                {
                    kernel[index] = 1.0f;
                }
                else
                {
                    kernel[index] = -1.0f;
                }
            }
        }
        }
}

void SimulationVisuals::submitToShader(Shader& shader) const
{
    shader.use();
    shader.setVec3("aliveCellColor", aliveColor[0], aliveColor[1], aliveColor[2]);
    shader.setVec3("deadCellColor", deadColor[0], deadColor[1], deadColor[2]);
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
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, rules.kernel.size() * sizeof(float), rules.kernel.data());
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, kernelSSBO); // binding = 2
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    randomize();
    submitRulesToShader();
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
    int updatesToPerform = static_cast<int>(simulationUpdateCounter * simulationUpdatesRate);
    if (updatesToPerform <= 0)
    {
        return 0;
    }

    simulationUpdateCounter -= (double)updatesToPerform / (double)simulationUpdatesRate;

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

void Simulation::submitRulesToShader()
{
	rules.submitToShader(*computeShader);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, kernelSSBO);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, rules.kernel.size() * sizeof(float), rules.kernel.data());
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void Simulation::submitVisualsToShader(Shader& shader)
{
	visuals.submitToShader(shader);
}

void Simulation::resetUpdatesCounter()
{
    simulationUpdateCounter = 0.0;
}
