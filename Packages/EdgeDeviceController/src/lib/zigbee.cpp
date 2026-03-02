#include <lib/zigbee.h>

ZigBee::ZigBee() :
    serial(SoftwareSerial(ZIGBEE_RX_PIN, ZIGBEE_TX_PIN)),
    inConfigMode(false) {}

ZigBee::~ZigBee() {}

void ZigBee::setup() {
    this->serial.begin(ZIGBEE_SERIAL_BAULT);
}

void ZigBee::enableConfigMode() {
    this->serial.println("+++");

    delay(1000); // TODO remove this if possible

    while (this->serial.available()) {} // TODO remove this if possible

    this->inConfigMode = true;
}

void ZigBee::setParam(String name, String value) {
    if (!this->inConfigMode) throw "You must enable the AT mode before";

    this->serial.print("AT" + name + value);
    delay(100); // TODO remove this if possible


}

String ZigBee::getParam(String name) {

}

void ZigBee::disableConfigMode() {

}

void ZigBee::waitUntilAvailable() {
    while (this->serial.available()) {} // TODO remove this if possible
}