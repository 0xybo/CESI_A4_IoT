#include <Arduino.h>
#include <main.h>

class ZigBee {
    SoftwareSerial serial;
    bool inConfigMode;

public:
    ZigBee();
    ~ZigBee();

    void setup();

    void enableConfigMode();
    void setParam(String name, String value);
    String getParam(String name);
    void disableConfigMode();

private:
    void waitUntilAvailable();
};