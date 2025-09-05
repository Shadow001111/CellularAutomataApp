#pragma once

class ColorPalette
{
	static void generateHSVForCells(float& h, float& sALive, float& sDead, float& vAlive, float& vDead);
public:
	static void HSVtoRGB(float h, float s, float v, float& r, float& g, float& b);

	static void generateMonochromatic(float* aliveColor, float* deadColor);
	static void generateAnalogous(float* aliveColor, float* deadColor);
	static void generateComplementary(float* aliveColor, float* deadColor);
	static void generateRandom(float* aliveColor, float* deadColor);
};

