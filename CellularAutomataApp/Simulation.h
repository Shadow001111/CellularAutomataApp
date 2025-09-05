#pragma once
#include "Texture2D.h"
#include "Shader.h"
#include <random>
#include <vector>
#include <memory>

struct SimulationSettings
{
	static const int MAX_NEIGHBOR_SEARCH_RANGE = 10;
    static const int KERNEL_MIN_VALUE = -2;
	static const int KERNEL_MAX_VALUE = 2;

    int neighborSearchRange = 1;
    float stableRange[2] = { 2, 3 };
    float birthRange[2] = { 3, 3 };
	std::vector<float> kernel;

	int previousNeighborSearchRange = 1;

    int simulationUpdatesRate = 60;

    SimulationSettings();

    void submitToShader(Shader& shader) const;
	float getMaxNeighborSum() const;
    void updateKernelSize();
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

    double simulationUpdateCounter = 0.0;
public:
    SimulationSettings settings;
    bool useTextureA = true;
    GLuint kernelSSBO;

    Simulation(int gridW, int gridH, Texture2D& texA, Texture2D& texB);
    void randomize();
    int update(double deltaTime);
	void updateSettingsInShader();
    void resetUpdatesCounter();
};