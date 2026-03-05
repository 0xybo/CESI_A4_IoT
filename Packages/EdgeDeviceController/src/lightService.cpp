#include <lightService.h>

int LightService::weather[2] = { 1, 1 };

void LightService::setup(LightSensor& lightSensor, Clock& clock, Led& led) {
    LightService::lightSensor = lightSensor;
    LightService::clock = clock;
    LightService::led = led;

    updateLed();
}

bool LightService::enabled = true;

void LightService::loop(int index) {
    if (!LightService::enabled) return;

    LightService::updateLed();

    // Serial.print(Clock::getFormattedDatetime());
    // Serial.print(" Light level: ");
    // Serial.print(LightService::lightSensor.read());
    // Serial.print("%");
    // Serial.print(" | Weather : ");
    // Serial.print(weather[0]);
    // Serial.print(",");
    // Serial.print(weather[1]);
    // Serial.print(" | LED currentBrightness: ");
    // Serial.print(LightService::led.getBrightness());
    // Serial.print("%");
    // Serial.print(" | LED currentTemperature: ");
    // Serial.print(LightService::led.getTemperature());
    // Serial.println("%");
}

void LightService::updateLed() {
    int minTemperature = 20;
    int maxTemperature = 60;

    int minBrightness = 20;
    int maxBrightness = 80;

    float lightLevel = LightService::lightSensor.read();
    float currentTemperature = LightService::led.getTemperature();
    float currentBrightness = LightService::led.getBrightness();
    float targetTemperature = constrain(lightLevel, minTemperature, maxTemperature);
    float targetBrightness = constrain((100 - lightLevel), minBrightness, maxBrightness);
    float nextTemperature = currentTemperature + (targetTemperature - currentTemperature) * 0.02; // Smooth transition
    float nextBrightness = currentBrightness + (targetBrightness - currentBrightness) * 0.02; // Smooth transition

    LightService::led.setTemperatureAndBrightness(nextTemperature, nextBrightness);
}

void LightService::setEnabled(bool enabled) {
    LightService::enabled = enabled;
}

void LightService::setWeather(int morning, int afternoon) {
    weather[0] = morning;
    weather[1] = afternoon;
}