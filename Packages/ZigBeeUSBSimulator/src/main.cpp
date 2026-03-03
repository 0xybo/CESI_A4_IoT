#include <main.h>

void setup() {
    Serial.begin(SERIAL_BAUD);
    delay(1000);

    // Initialize cloud coordinator
    ZigBee::setup();
}

void loop() {
    static int index = 0;

    // Handle ZigBee events
    ZigBee::loop(index);

    index = (index + 1) % 100; // Loop index for periodic tasks
    delay(10);
}