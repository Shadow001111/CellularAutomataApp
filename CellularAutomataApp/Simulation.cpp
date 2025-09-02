#include "Simulation.h"

Simulation::Simulation(int gridW, int gridH, Texture2D& texA, Texture2D& texB)
    : gridW(gridW), gridH(gridH), textureA(texA), textureB(texB), gen(rd()), dis(0, 1)
{
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
}