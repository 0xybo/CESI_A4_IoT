#include <main.h>

bool isConfig = false;

void setup() {
    Serial.begin(SERIAL_BAUD);
    delay(1000); // Wait for Serial to initialize
    Clock::setup();

    Led::setup();

    SwitchButton::setup();
    SwitchButton::onPress(
        [](bool active) {
            Serial.print(F("Switch is now "));
            Serial.println(active ? F("active") : F("inactive"));
            LightService::setEnabled(active);
            if (!active) Led::setTemperatureAndBrightness(0, 0);
            else Led::setTemperatureAndBrightness(50, 50);
        }
    );

    ZigBee::setup();

    Cloud::setup();
    Cloud::subscribe("iot/settings",
        [](const XBeeAddress64& sender, uint8_t* payload, uint8_t length) {
            unsigned long timestamp = strtoul((const char*)payload, NULL, 10);
            Datetime dt(timestamp);
            // Serial.print(timestamp);
            // Serial.print(" ");
            // Serial.println(dt.getFormattedDatetime());
            Clock::setDatetime(dt);
            isConfig = true;
        }
    );
    Cloud::subscribe("iot/weather",
        [](const XBeeAddress64& sender, uint8_t* payload, uint8_t length) {
            LightService::setWeather(payload[0], payload[1]);
        }
    );
}

void loop() {
    static int index = 0;
    static unsigned long long previousTimestamp = 0;

    SwitchButton::loop(index);
    LightService::loop(index);
    ZigBee::loop(index);
    Cloud::loop(index);

    unsigned long long currentTimestamp = millis();
    if ((currentTimestamp - previousTimestamp) > 10000) {
        previousTimestamp = currentTimestamp;
        if (isConfig) {
            char* timestamp;
            ulltoa(Clock::getTimestamp(), timestamp);
            String payload = timestamp;
            payload += ";";
            payload += Led::getTemperature();
            payload += ";";
            payload += Led::getBrightness();
            payload += ";";
            payload += LightSensor::read();
            Cloud::publish("iot/stats", payload.c_str());
        }
        else {
            Serial.println("Waiting config");
            Cloud::publish("iot/ask_settings", "");
        }
    }

    index = (index + 1) % 1001;
    delay(10);
}