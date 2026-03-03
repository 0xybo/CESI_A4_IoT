#include <main.h>

void setup() {
    Serial.begin(SERIAL_BAUD);
    delay(1000); // Wait for Serial to initialize
    // Clock::setup();

    // Led::setup();

    // SwitchButton::setup();
    // SwitchButton::onPress(
    //     [](bool active) {
    //         Serial.print(F("Switch is now "));
    //         Serial.println(active ? F("active") : F("inactive"));
    //         LightService::setEnabled(active);
    //         if (!active) Led::setTemperatureAndBrightness(0, 0);
    //     }
    // );

    ZigBee::setup();
}

void loop() {
    static int index = 0;

    // SwitchButton::loop(index);
    // LightService::loop(index);
    ZigBee::loop(index);

    index = (index + 1) % 101;
    delay(10);
}