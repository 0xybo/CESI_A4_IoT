#include <main.h>

void setup() {
    Serial.begin(SERIAL_BAUD);
    delay(1000); // Wait for Serial to initialize
    Serial.println(F("Starting setup..."));
    Clock::setup();

    Led::setup();

    SwitchButton::setup();
    SwitchButton::onPress(
        [](bool active) {
            Serial.print(F("Switch is now "));
            Serial.println(active ? F("active") : F("inactive"));
            LightService::setEnabled(active);
            if (!active) Led::setTemperatureAndBrightness(0, 0);
        }
    );

    Serial.println(F("Setup complete"));
}

void loop() {
    delay(100);
    // Serial.print(Clock::getFormattedDatetime());
    // Serial.print(' ');
    // Serial.println(LightSensor::read());

    SwitchButton::loop();
    LightService::loop();
}