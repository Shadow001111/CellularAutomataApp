#include "ColorPalette.h"
#include <math.h>
#include "Random.h"


static float linearToSRGB(float x)
{
    return x <= 0.0031308f ? 12.92f * x : 1.055f * powf(x, 1.0f / 2.4f) - 0.055f;
}

static float clamp01(float x)
{
    if (x < 0.0f) return 0.0f;
    else if (x > 1.0f) return 1.0f;
    return x;
}

void ColorPalette::generateHSVForCells(float& h, float& sALive, float& sDead, float& vAlive, float& vDead)
{
    h = Random::Float(0.0f, 1.0f);
    sALive = Random::Float(0.5f, 1.0f);
    sDead = Random::Float(0.5f, 1.0f);
    vAlive = Random::Float(0.75f, 1.0f);
	vDead = Random::Float(0.25f, 0.5f);
}

void ColorPalette::generateOKLabForCells(float& L, float& aAlive, float& aDead, float& bAlive, float& bDead)
{
    L = 0.8f;
    aAlive = Random::Float(-0.4f, 0.4f);
    bAlive = Random::Float(-0.4f, 0.4f);

    // Dead state shifted slightly toward gray/desaturated
    float fade = Random::Float(0.2f, 0.6f);
    aDead = aAlive * fade;
    bDead = bAlive * fade;
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

void ColorPalette::OKLabToRGB(float L, float a, float b, float& R, float& G, float& B)
{
    // Convert OKLab to LMS
    float l_ = L + 0.3963377774f * a + 0.2158037573f * b;
    float m_ = L - 0.1055613458f * a - 0.0638541728f * b;
    float s_ = L - 0.0894841775f * a - 1.2914855480f * b;

    float l = l_ * l_ * l_;
    float m = m_ * m_ * m_;
    float s = s_ * s_ * s_;

    // LMS to linear sRGB
    float r = +4.0767416621f * l - 3.3077115913f * m + 0.2309699292f * s;
    float g = -1.2684380046f * l + 2.6097574011f * m - 0.3413193965f * s;
    float b2 = -0.0041960863f * l - 0.7034186147f * m + 1.7076147010f * s;

    // Linear to gamma corrected sRGB
    R = clamp01(linearToSRGB(r));
    G = clamp01(linearToSRGB(g));
    B = clamp01(linearToSRGB(b2));
}


void ColorPalette::generateMonochromaticHSV(float* aliveColor, float* deadColor)
{
	float hue, sAlive, sDead, vAlive, vDead;
    generateHSVForCells(hue, sAlive, sDead, vAlive, vDead);

	HSVtoRGB(hue, sAlive, vAlive, aliveColor[0], aliveColor[1], aliveColor[2]);
	HSVtoRGB(hue, sDead, vDead, deadColor[0], deadColor[1], deadColor[2]);
}

void ColorPalette::generateAnalogousHSV(float* aliveColor, float* deadColor)
{
    float hue, sAlive, sDead, vAlive, vDead;
    generateHSVForCells(hue, sAlive, sDead, vAlive, vDead);
    float analogousHue = hue + Random::Float(1.0f / 12.0f, 1.0f / 3.0f); // 30 to 120 degrees apart

    HSVtoRGB(hue, sAlive, vAlive, aliveColor[0], aliveColor[1], aliveColor[2]);
    HSVtoRGB(analogousHue, sDead, vDead, deadColor[0], deadColor[1], deadColor[2]);
}

void ColorPalette::generateComplementaryHSV(float* aliveColor, float* deadColor)
{
    float hue, sAlive, sDead, vAlive, vDead;
    generateHSVForCells(hue, sAlive, sDead, vAlive, vDead);
    float complementaryHue = hue + 0.5f; // 180 degrees apart

    HSVtoRGB(hue, sAlive, vAlive, aliveColor[0], aliveColor[1], aliveColor[2]);
    HSVtoRGB(complementaryHue, sDead, vDead, deadColor[0], deadColor[1], deadColor[2]);
}

void ColorPalette::generateRandomHSV(float* aliveColor, float* deadColor)
{
	int choice = Random::Int(1, 3);
    switch (choice)
    {
        case 1:
            generateMonochromaticHSV(aliveColor, deadColor);
            break;
        case 2:
            generateAnalogousHSV(aliveColor, deadColor);
            break;
        case 3:
            generateComplementaryHSV(aliveColor, deadColor);
            break;
        default:
            generateMonochromaticHSV(aliveColor, deadColor);
            break;
	}
}

void ColorPalette::generateMonochromaticOKLab(float* aliveColor, float* deadColor)
{
    float L, aAlive, aDead, bAlive, bDead;
    generateOKLabForCells(L, aAlive, aDead, bAlive, bDead);

    OKLabToRGB(L, aAlive, bAlive, aliveColor[0], aliveColor[1], aliveColor[2]);
    OKLabToRGB(L * 0.5f, aDead, bDead, deadColor[0], deadColor[1], deadColor[2]);
}

void ColorPalette::generateAnalogousOKLab(float* aliveColor, float* deadColor)
{
    float L, aAlive, aDead, bAlive, bDead;
    generateOKLabForCells(L, aAlive, aDead, bAlive, bDead);

    // Rotate in ab-plane for analogous color
    float angle = 45.0f * (3.14159265f / 180.0f); // radians
    float cosA = cosf(angle), sinA = sinf(angle);

    float aRot = aDead * cosA - bDead * sinA;
    float bRot = aDead * sinA + bDead * cosA;

    OKLabToRGB(L, aAlive, bAlive, aliveColor[0], aliveColor[1], aliveColor[2]);
    OKLabToRGB(L * 0.5f, aRot, bRot, deadColor[0], deadColor[1], deadColor[2]);
}

void ColorPalette::generateComplementaryOKLab(float* aliveColor, float* deadColor)
{
    float L, aAlive, aDead, bAlive, bDead;
    generateOKLabForCells(L, aAlive, aDead, bAlive, bDead);

    // Opposite point in ab-plane
    float aComp = -aDead;
    float bComp = -bDead;

    OKLabToRGB(L, aAlive, bAlive, aliveColor[0], aliveColor[1], aliveColor[2]);
    OKLabToRGB(L * 0.5f, aComp, bComp, deadColor[0], deadColor[1], deadColor[2]);
}

void ColorPalette::generateRandomOKLab(float* aliveColor, float* deadColor)
{
    int choice = Random::Int(1, 3);
    switch (choice)
    {
        case 1:
            generateMonochromaticOKLab(aliveColor, deadColor);
            break;
        case 2:
            generateAnalogousOKLab(aliveColor, deadColor);
            break;
        case 3:
            generateComplementaryOKLab(aliveColor, deadColor);
            break;
        default:
            generateMonochromaticOKLab(aliveColor, deadColor);
            break;
	}
}
