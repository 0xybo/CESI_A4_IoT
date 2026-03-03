#include <Arduino.h>

// ------------------------------------------------------------
//  Utilities
// ------------------------------------------------------------
struct RGB { uint8_t r, g, b; };
struct HSL { uint16_t h; uint8_t s, l; };

float clampf(float x, float a, float b) {
    return (x < a) ? a : (x > b) ? b : x;
}

RGB interpRGB(const RGB& c0, const RGB& c1, float t) {
    RGB out;
    out.r = c0.r + (c1.r - c0.r) * t;
    out.g = c0.g + (c1.g - c0.g) * t;
    out.b = c0.b + (c1.b - c0.b) * t;
    return out;
}

RGB hexToRgb(uint32_t hex) {
    RGB c;
    c.r = (hex >> 16) & 0xFF;
    c.g = (hex >> 8) & 0xFF;
    c.b = hex & 0xFF;
    return c;
}

// ------------------------------------------------------------
//  RGB → HSL (integer‑friendly)
// ------------------------------------------------------------
HSL rgbToHsl(const RGB& c) {
    float r = c.r / 255.0f;
    float g = c.g / 255.0f;
    float b = c.b / 255.0f;

    float maxv = max(r, max(g, b));
    float minv = min(r, min(g, b));
    float h, s, l = (maxv + minv) * 0.5f;

    if (maxv == minv) {
        h = s = 0;
    }
    else {
        float d = maxv - minv;
        s = (l > 0.5f) ? d / (2.0f - maxv - minv) : d / (maxv + minv);

        if (maxv == r)      h = (g - b) / d + (g < b ? 6 : 0);
        else if (maxv == g) h = (b - r) / d + 2;
        else                h = (r - g) / d + 4;

        h *= 60.0f;
    }

    HSL out;
    out.h = (uint16_t)(h + 0.5f);
    out.s = (uint8_t)(s * 100 + 0.5f);
    out.l = (uint8_t)(l * 100 + 0.5f);
    return out;
}

// ------------------------------------------------------------
//  Spectre stops (same as JS)
// ------------------------------------------------------------
struct Stop { uint16_t t; uint32_t hex; };

const Stop spectreStops[] = {
    {2500, 0xD6BC28},
    {3000, 0xEBDB5F},
    {3500, 0xF8F2D2},
    {4000, 0xFDFDF9},
    {4500, 0xFEFEFE},
    {5000, 0xE9F4FF},
    {5500, 0xCAEBFF},
    {6000, 0xA8DFFA},
    {6500, 0x83CAF2}
};
const uint8_t STOP_COUNT = sizeof(spectreStops) / sizeof(Stop);

// ------------------------------------------------------------
//  Main function
// ------------------------------------------------------------
enum ColorMode { MODE_RGB, MODE_HSL };

void getColorFromSpectre(
    uint16_t temperature,
    uint8_t brightness,
    uint16_t minTemp,
    uint16_t maxTemp,
    ColorMode mode,
    RGB& outRgb,
    HSL& outHsl
) {
    // clamp temperature inside bounds
    float t = clampf(temperature, minTemp, maxTemp);

    // find spectre interval
    uint8_t i = 1;
    while (i < STOP_COUNT && t > spectreStops[i].t) i++;

    const Stop& prev = spectreStops[i - 1];
    const Stop& next = (i < STOP_COUNT) ? spectreStops[i] : spectreStops[i - 1];

    float frac = (t - prev.t) / float(next.t - prev.t);

    RGB c0 = hexToRgb(prev.hex);
    RGB c1 = hexToRgb(next.hex);

    // interpolate
    RGB rgb = interpRGB(c0, c1, frac);

    // apply brightness
    float bn = brightness / 100.0f;
    rgb.r = rgb.r * bn;
    rgb.g = rgb.g * bn;
    rgb.b = rgb.b * bn;

    outRgb = rgb;

    if (mode == MODE_HSL) {
        outHsl = rgbToHsl(rgb);
    }
}

void loop() {
    uint16_t temp = 4200;     // Kelvin
    uint8_t bright = 75;      // %
    uint16_t minT = 3000;
    uint16_t maxT = 5800;

    RGB rgb;
    HSL hsl;

    getColorFromSpectre(temp, bright, minT, maxT, MODE_RGB, rgb, hsl);

    Serial.print("RGB = ");
    Serial.print(rgb.r); Serial.print(",");
    Serial.print(rgb.g); Serial.print(",");
    Serial.println(rgb.b);

    getColorFromSpectre(temp, bright, minT, maxT, MODE_HSL, rgb, hsl);

    Serial.print("HSL = ");
    Serial.print(hsl.h); Serial.print(",");
    Serial.print(hsl.s); Serial.print(",");
    Serial.println(hsl.l);

    delay(1000);
}

