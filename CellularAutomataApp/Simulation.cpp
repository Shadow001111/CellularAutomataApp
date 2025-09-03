#include "Simulation.h"
#include <glad/glad.h>
#include <math.h>

const int WORK_GROUP_W = 8;
const int WORK_GROUP_H = 8;

Simulation::Simulation(int gridW, int gridH, Texture2D& texA, Texture2D& texB)
    : gridW(gridW), gridH(gridH), textureA(texA), textureB(texB), gen(rd()), dis(0, 1)
{
    std::vector<Shader::ShaderSource> sources = {
        { GL_COMPUTE_SHADER, "Shaders/automata.comp" }
    };
    computeShader = std::make_unique<Shader>(sources);
}

void Simulation::randomize(bool useTextureA)
{
    auto& currentTexture = useTextureA ? textureA : textureB;
    std::vector<uint8_t> data(gridW * gridH);
    for (int i = 0; i < gridW * gridH; ++i)
    {
        data[i] = static_cast<uint8_t>(dis(gen));
    }
    currentTexture.setData(data.data());
}

void Simulation::update(bool useTextureA)
{
    const auto& currentWorld = useTextureA ? textureA : textureB;
    const auto& nextWorld = useTextureA ? textureB : textureA;

	// Use compute shader for calculating next world state
    computeShader->use();

    // Bind textures to image units
    glBindImageTexture(0, currentWorld.getID(), 0, GL_FALSE, 0, GL_READ_ONLY, currentWorld.getInternalFormat());
    glBindImageTexture(1, nextWorld.getID(), 0, GL_FALSE, 0, GL_WRITE_ONLY, nextWorld.getInternalFormat());

    // Send grid size using uniform
    computeShader->setInt("gridWidth", gridW);
    computeShader->setInt("gridHeight", gridH);

    // Run compute shader
	int groupsX = ceilf(gridW / WORK_GROUP_W);
	int groupsY = ceilf(gridH / WORK_GROUP_H);
    glDispatchCompute((GLuint)groupsX, (GLuint)groupsY, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}