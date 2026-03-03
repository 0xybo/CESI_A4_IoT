#pragma once
#include <Arduino.h>
#include <constants.h>
#include <lib/lightSensor.h>
#include <lib/clock.h>
#include <lib/led.h>

class LightService {
private:
    static LightSensor& lightSensor;
    static Clock& clock;
    static Led& led;
    static bool enabled;

public:
    static void setup(LightSensor& lightSensor, Clock& clock, Led& led);
    static void loop();
    static void setEnabled(bool enabled);
};