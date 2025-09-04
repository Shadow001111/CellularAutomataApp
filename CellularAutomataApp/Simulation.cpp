#include "Simulation.h"
#include <glad/glad.h>
#include <math.h>

const int WORK_GROUP_W = 8;
const int WORK_GROUP_H = 8;


void SimulationSettings::submitToShader(Shader& shader) const
{
    shader.use();
    shader.setInt("neighborSearchRange", neighborSearchRange);
    shader.setInt("countTheCenterCell", countTheCenterCell ? 1 : 0);
    shader.setUvec2("stableRange", stableRange[0], stableRange[1]);
    shader.setUvec2("birthRange", birthRange[0], birthRange[1]);
}

int SimulationSettings::getMaxNeighborCount() const
{
	int diameter = neighborSearchRange * 2 + 1;
	int totalCells = diameter * diameter;
    if (!countTheCenterCell)
    {
		totalCells -= 1;
    }
    return totalCells;
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

void Simulation::update(int updates)
{
    // Use compute shader for calculating next world state
    computeShader->use();

    GLuint aID = textureA.getID();
    GLuint bID = textureB.getID();

    for (int i = 0; i < updates; i++)
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
}

void Simulation::updateSettingsInShader()
{
	settings.submitToShader(*computeShader);
}
