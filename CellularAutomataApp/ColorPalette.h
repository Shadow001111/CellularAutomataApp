#pragma once

class ColorPalette
{
public:
	static void HSVtoRGB(float h, float s, float v, float& r, float& g, float& b);

	static void generateMonochromatic(float* aliveColor, float* deadColor);
	static void generateAnalogous(float* aliveColor, float* deadColor);
	static void generateComplementary(float* aliveColor, float* deadColor);
	static void generateRandom(float* aliveColor, float* deadColor);
};

