#include <lib/led.h>

ChainableLED Led::led = ChainableLED(LED_CLK_PIN, LED_DATA_PIN, LED_QUANTITY);
float Led::brightness = 50;
float Led::temperature = 50;

void Led::updateColor() {
    float r, g, b;

    // Red
    if (Led::temperature <= 66) r = 255;
    else r = 329.698727446 * pow(Led::temperature - 60, -0.1332047592);

    // Green
    if (Led::temperature <= 66)
        g = 99.4708025861 * log(Led::temperature) - 161.1195681661;
    else
        g = 288.1221695283 * pow(Led::temperature - 60, -0.0755148492);

    // Blue
    if (Led::temperature >= 66) b = 255;
    else if (Led::temperature <= 19) b = 0;
    else b = 138.5177312231 * log(Led::temperature - 10) - 305.0447927307;

    float brightness = mapFloat(Led::brightness, 0, 100, 0, 1);

    // Clamp to [0,255]
    r = mapFloat(r, 0, 255, 0, 255) * brightness;
    g = mapFloat(g, 0, 255, 0, 255) * brightness;
    b = mapFloat(b, 0, 255, 0, 255) * brightness;

    Led::led.setColorRGB(LED_NUMBER, r, g, b);
}


void Led::setup() {
    Led::led.init();
    Led::updateColor();
}

void Led::loop() {

}

float Led::getBrightness() {
    return Led::brightness;
}

float Led::getTemperature() {
    return Led::temperature;
}

void Led::setBrightness(float brightness) {
    Led::brightness = brightness;
    Led::updateColor();
}

void Led::setTemperature(float temperature) {
    Led::temperature = temperature;
    Led::updateColor();
}

void Led::setTemperatureAndBrightness(float temperature, float brightness) {
    Led::temperature = temperature;
    Led::brightness = brightness;
    Led::updateColor();
}