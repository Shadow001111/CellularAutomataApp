#pragma once
#include "Texture2D.h"
#include "Shader.h"
#include <random>
#include <vector>
#include <memory> // For std::unique_ptr

class Simulation
{
public:
    Simulation(int gridW, int gridH,Texture2D& texA, Texture2D& texB);
    void randomize(bool useTextureA);
    void update(bool useTextureA);

private:
    int gridW = 0;
    int gridH = 0;
    Texture2D& textureA;
    Texture2D& textureB;
    std::random_device rd;
    std::mt19937 gen;
    std::uniform_int_distribution<> dis;

    std::unique_ptr<Shader> computeShader;
};