#pragma once

class ColorPalette
{
	static void generateHSVForCells(float& h, float& sALive, float& sDead, float& vAlive, float& vDead);
	static void generateOKLabForCells(float& L, float& aAlive, float& aDead, float& bAlive, float& bDead);
public:
	static void HSVtoRGB(float h, float s, float v, float& r, float& g, float& b);
	static void OKLabToRGB(float L, float a, float b, float& R, float& G, float& B);

	static void generateMonochromaticHSV(float* aliveColor, float* deadColor);
	static void generateAnalogousHSV(float* aliveColor, float* deadColor);
	static void generateComplementaryHSV(float* aliveColor, float* deadColor);
	static void generateRandomHSV(float* aliveColor, float* deadColor);

	static void generateMonochromaticOKLab(float* aliveColor, float* deadColor);
	static void generateAnalogousOKLab(float* aliveColor, float* deadColor);
	static void generateComplementaryOKLab(float* aliveColor, float* deadColor);
	static void generateRandomOKLab(float* aliveColor, float* deadColor);
};