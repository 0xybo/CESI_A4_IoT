#pragma once
#include <Arduino.h>
#include <constants.h>
#include <utils/float.h>

class LightSensor {
private:
    static float lastReading;

public:
    /**
     * Reads the current light level from the sensor and returns it as a percentage (0-100%).
     * The raw sensor value is mapped from the range 0-800 to 0-100
     */
    static float read();
};
