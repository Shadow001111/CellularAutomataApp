#include "ColorPalette.h"
#include <math.h>
#include "Random.h"

void ColorPalette::generateHSVForCells(float& h, float& sALive, float& sDead, float& vAlive, float& vDead)
{
    h = Random::Float(0.0f, 1.0f);
    sALive = Random::Float(0.5f, 1.0f);
    sDead = Random::Float(0.5f, 1.0f);
    vAlive = Random::Float(0.75f, 1.0f);
	vDead = Random::Float(0.25f, 0.5f);
}

void ColorPalette::HSVtoRGB(float h, float s, float v, float& r, float& g, float& b)
{
    if (s == 0.0f)
    {
        r = g = b = v;
        return;
    }

    h = fmodf(h, 1.0f) * 6.0f;
    int i = (int)h;
    float f = h - (float)i;
    float p = v * (1.0f - s);
    float q = v * (1.0f - s * f);
    float t = v * (1.0f - s * (1.0f - f));

    switch (i)
    {
        case 0: r = v; g = t; b = p; break;
        case 1: r = q; g = v; b = p; break;
        case 2: r = p; g = v; b = t; break;
        case 3: r = p; g = q; b = v; break;
        case 4: r = t; g = p; b = v; break;
        case 5: default: r = v; g = p; b = q; break;
    }
}

void ColorPalette::generateMonochromatic(float* aliveColor, float* deadColor)
{
	float hue, sAlive, sDead, vAlive, vDead;
    generateHSVForCells(hue, sAlive, sDead, vAlive, vDead);

	HSVtoRGB(hue, sAlive, vAlive, aliveColor[0], aliveColor[1], aliveColor[2]);
	HSVtoRGB(hue, sDead, vDead, deadColor[0], deadColor[1], deadColor[2]);
}

void ColorPalette::generateAnalogous(float* aliveColor, float* deadColor)
{
    float hue, sAlive, sDead, vAlive, vDead;
    generateHSVForCells(hue, sAlive, sDead, vAlive, vDead);
    float analogousHue = hue + Random::Float(1.0f / 12.0f, 1.0f / 3.0f); // 30 to 120 degrees apart

    HSVtoRGB(hue, sAlive, vAlive, aliveColor[0], aliveColor[1], aliveColor[2]);
    HSVtoRGB(analogousHue, sDead, vDead, deadColor[0], deadColor[1], deadColor[2]);
}

void ColorPalette::generateComplementary(float* aliveColor, float* deadColor)
{
    float hue, sAlive, sDead, vAlive, vDead;
    generateHSVForCells(hue, sAlive, sDead, vAlive, vDead);
    float complementaryHue = hue + 0.5f; // 180 degrees apart

    HSVtoRGB(hue, sAlive, vAlive, aliveColor[0], aliveColor[1], aliveColor[2]);
    HSVtoRGB(complementaryHue, sDead, vDead, deadColor[0], deadColor[1], deadColor[2]);
}

void ColorPalette::generateRandom(float* aliveColor, float* deadColor)
{
	int choice = Random::Int(1, 3);
    switch (choice)
    {
        case 1:
            generateMonochromatic(aliveColor, deadColor);
            break;
        case 2:
            generateAnalogous(aliveColor, deadColor);
            break;
        case 3:
            generateComplementary(aliveColor, deadColor);
            break;
        default:
            generateMonochromatic(aliveColor, deadColor);
            break;
	}
}
