#include <lib/zigbee.h>

SoftwareSerial zigBeeSerial(ZIGBEE_RX_PIN, ZIGBEE_TX_PIN);
bool isATMode = false;

void setupZigBee() {
    zigBeeSerial.begin(ZIGBEE_SERIAL_BAULT);
}

void enableATMode() {
    zigBeeSerial.println("+++");

    delay(1000); // TODO remove this if possible

    while (zigBeeSerial.available()) {} // TODO remove this if possible

    isATMode = true;
}

void setZigBeeParam(String name, String value) {
    if (!isATMode) throw "You must enable the AT mode before";

    zigBeeSerial.print("AT" + name + value);
    delay(100); // TODO remove this if possible

    while (zigBeeSerial.available()) {} // TODO remove this if possible
}

void getZigBeeParam(String name) {
    // TODO
}

void disableATMode() {

}