#pragma once
#include "Texture2D.h"
#include "Shader.h"
#include <random>
#include <vector>
#include <memory>

struct SimulationSettings
{
    int neighborSearchRange = 1;
    bool countTheCenterCell = false;
    int stableRange[2] = { 2, 3 };
    int birthRange[2] = { 3, 3 };

    void submitToShader(Shader& shader) const;
	int getMaxNeighborCount() const;
};

class Simulation
{
    int gridW = 0;
    int gridH = 0;
    GLuint groupsX, groupsY;

    Texture2D& textureA;
    Texture2D& textureB;

    std::random_device rd;
    std::mt19937 gen;
    std::uniform_int_distribution<> dis;

    std::unique_ptr<Shader> computeShader;
public:
    SimulationSettings settings;
    bool useTextureA = true;

    Simulation(int gridW, int gridH, Texture2D& texA, Texture2D& texB);
    void randomize();
    void update(int updates);
	void updateSettingsInShader();
};