#include <lightService.h>

void LightService::setup(LightSensor& lightSensor, Clock& clock, Led& led) {
    LightService::lightSensor = lightSensor;
    LightService::clock = clock;
    LightService::led = led;
}

bool LightService::enabled = true;

void LightService::loop(int index) {
    if (!LightService::enabled) return;

    float lightLevel = LightService::lightSensor.read();
    float currentTemperature = LightService::led.getTemperature();
    float currentBrightness = LightService::led.getBrightness();
    float targetTemperature = lightLevel;
    float targetBrightness = 100 - lightLevel;
    float nextTemperature = currentTemperature + (targetTemperature - currentTemperature) * 0.02; // Smooth transition
    float nextBrightness = currentBrightness + (targetBrightness - currentBrightness) * 0.02; // Smooth transition

    if (index % 10 == 0) { // Log every 10 iterations (every 100ms if loop runs every 10ms)
        Serial.print(Clock::getFormattedDatetime());
        Serial.print(" Light level: ");
        Serial.print(lightLevel);
        Serial.print("%");
        Serial.print(" | LED currentBrightness: ");
        Serial.print(LightService::led.getBrightness());
        Serial.print("%");
        Serial.print(" | LED currentTemperature: ");
        Serial.print(LightService::led.getTemperature());
        Serial.println("°C");
    }

    LightService::led.setTemperatureAndBrightness(nextTemperature, nextBrightness);
}

void LightService::setEnabled(bool enabled) {
    LightService::enabled = enabled;
}