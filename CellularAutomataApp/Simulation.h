#pragma once
#include "Texture2D.h"
#include "Shader.h"
#include <random>
#include <vector>
#include <memory>

struct SimulationRules
{
	static const int MAX_NEIGHBOR_SEARCH_RANGE = 10;
    static const int KERNEL_MIN_VALUE = -5;
	static const int KERNEL_MAX_VALUE = 5;

    int neighborSearchRange = 1;
    float stableRange[2] = { 2, 3 };
    float birthRange[2] = { 3, 3 };
	std::vector<float> kernel;

	int previousNeighborSearchRange = 1;

    SimulationRules();

    void submitToShader(Shader& shader) const;
	float getMaxNeighborSum() const;
    void updateKernelSize();
	void randomizeKernel();
};

struct SimulationVisuals
{
	float aliveColor[3] = { 1.0f, 1.0f, 1.0f };
	float deadColor[3] = { 0.0f, 0.0f, 0.0f };

    void submitToShader(Shader& shader) const;
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
    SimulationRules rules;
	SimulationVisuals visuals;
    bool useTextureA = true;
    GLuint kernelSSBO;
    bool isRunning = true;
    int simulationUpdatesRate = 60;

    Simulation(int gridW, int gridH, Texture2D& texA, Texture2D& texB);
    void randomize();
    int update(double deltaTime);
	void submitRulesToShader();
	void submitVisualsToShader(Shader& shader);
    void resetUpdatesCounter();
};