#include <main.h>

void setup() {
    Serial.begin(SERIAL_BAUD);
    delay(1000);

    // Initialize cloud coordinator
    ZigBee::setup();
    Cloud::setup();
    Cloud::subscribe("iot/stats",
        [](XBeeAddress64& sender, uint8_t* payload, uint8_t length) {
            Serial.print("iot/stats;");
            Serial.print(sender.getMsb(), HEX);
            Serial.print(sender.getLsb(), HEX);
            Serial.print(";");
            for (int i = 0; i < length; i++) {
                Serial.print((char)payload[i]);
            }
            Serial.println();
        }
    );
    Cloud::subscribe("iot/ask_settings",
        [](XBeeAddress64& sender, uint8_t* payload, uint8_t length) {
            Serial.print("iot/ask_settings;");
            Serial.print(sender.getMsb(), HEX);
            Serial.print(sender.getLsb(), HEX);
            Serial.print(";");
            for (int i = 0; i < length; i++) {
                Serial.print((char)payload[i]);
            }
            Serial.println();
        }
    );
}

void loop() {
    static int index = 0;

    // Handle ZigBee events
    ZigBee::loop(index);
    Cloud::loop(index);

    static char buffer[50];
    static int bufferIndex = 0;
    while (Serial.available()) {
        char c = (char)Serial.read();
        // Serial.print(c);

        // Handle end of line (newline or carriage return)
        if (c == '\n') {
            if (bufferIndex > 0) {
                buffer[bufferIndex] = '\0'; // Null-terminate

                // Parse channel and payload separated by space
                int separatorIndex = -1;
                for (int i = 0; i < bufferIndex; i++) {
                    if (buffer[i] == ';') {
                        separatorIndex = i;
                        break;
                    }
                }

                if (separatorIndex > 0) {
                    // Extract channel (before space)
                    buffer[separatorIndex] = '\0';
                    const char* channel = buffer;

                    // Extract payload (after space)
                    const char* payload = buffer + separatorIndex + 1;

                    // Publish to cloud
                    Cloud::publish(channel, payload);
                    // Serial.println("Sent");
                }

                bufferIndex = 0;
            }
        }
        else if (bufferIndex < 50) {
            buffer[bufferIndex++] = c;
        }
    }

    index = (index + 1) % 101; // Loop index for periodic tasks
    delay(10);
}