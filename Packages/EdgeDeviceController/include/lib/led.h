#pragma once
#include <Arduino.h>
#include <constants.h>
#include <ChainableLED.h>
#include <utils/float.h>

class Led {
private:
    static ChainableLED led;
    static float brightness;
    static float temperature;
    static void updateColor();

public:
    static void setup();
    static void loop();
    static float getBrightness();
    static float getTemperature();
    static void setBrightness(float brightness);
    static void setTemperature(float temperature);
    static void setTemperatureAndBrightness(float temperature, float brightness);
};