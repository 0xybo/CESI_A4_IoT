#include <lightService.h>

void LightService::setup(LightSensor& lightSensor, Clock& clock, Led& led) {
    LightService::lightSensor = lightSensor;
    LightService::clock = clock;
    LightService::led = led;
}

bool LightService::enabled = true;

void LightService::loop() {
    if (!LightService::enabled) return;

    float lightLevel = LightService::lightSensor.read();
    float temperature = lightLevel;
    float brightness = 100 - lightLevel;

    LightService::led.setTemperatureAndBrightness(temperature, brightness);

    Serial.print(Clock::getFormattedDatetime());
    Serial.print(" Light level: ");
    Serial.print(lightLevel);
    Serial.print("%");
    Serial.print(" | LED brightness: ");
    Serial.print(LightService::led.getBrightness());
    Serial.print("%");
    Serial.print(" | LED temperature: ");
    Serial.print(LightService::led.getTemperature());
    Serial.println("°C");
}

void LightService::setEnabled(bool enabled) {
    LightService::enabled = enabled;
}