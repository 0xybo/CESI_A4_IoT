#include <lib/zigBee.h>

// definitions of static members
SoftwareSerial ZigBee::serial(ZIGBEE_RX_PIN, ZIGBEE_TX_PIN);
XBee ZigBee::xbee;

// Cloud static data
Cloud::Subscription Cloud::subscriptions[10];
uint8_t Cloud::subscriptionCount = 0;

void ZigBee::startParamSession() {
    serial.print("+++");
    delay(1000);
    // while (serial.available()) {
    //     Serial.print((char)serial.read());
    // }
    // Serial.println();
    while (serial.available()) serial.read();
}

void ZigBee::setParam(const char* command, const char* parameter) {
    serial.print("AT");
    serial.print(command);
    serial.println(parameter);
    delay(100);
    // while (serial.available()) {
    //     Serial.print((char)serial.read());
    // }
    // Serial.println();
    while (serial.available()) serial.read();
}

void ZigBee::endParamSession() {
    serial.println("ATCN");
    delay(100);
}

void ZigBee::processPacket() {
    XBeeResponse& rsp = xbee.getResponse();
    switch (rsp.getApiId()) {
        case ZB_RX_RESPONSE: {
                ZBRxResponse rx;
                rsp.getZBRxResponse(rx);
                handleRxPacket(rx);
                break;
            }
        case ZB_TX_STATUS_RESPONSE: {
                ZBTxStatusResponse tx;
                rsp.getZBTxStatusResponse(tx);
                handleTxStatus(tx);
                break;
            }
        case AT_COMMAND_RESPONSE: {
                AtCommandResponse at;
                rsp.getAtCommandResponse(at);
                handleATResponse(at);
                break;
            }
        case REMOTE_AT_COMMAND_RESPONSE: {
                RemoteAtCommandResponse rat;
                rsp.getRemoteAtCommandResponse(rat);
                handleRemoteATResponse(rat);
                break;
            }
        case MODEM_STATUS_RESPONSE: {
                ModemStatusResponse ms;
                rsp.getModemStatusResponse(ms);
                handleModemStatus(ms);
                break;
            }
        default:
            handleUnknownPacket();
            break;
    }
}

void ZigBee::handleRxPacket(ZBRxResponse& resp) {
    XBeeAddress64 addr = resp.getRemoteAddress64();
    uint8_t length = resp.getDataLength();

    // Serial.print("RX Packet Received, ");
    // Serial.print("From: 0x");
    // Serial.print((unsigned long)addr.getMsb(), HEX);
    // Serial.print(", Payload: ");
    // for (uint8_t i = 0; i < length; i++) {
    //     Serial.print((char)resp.getData()[i]);
    // }
    // Serial.println();

    // deliver to any cloud subscribers
    Cloud::deliver(addr, (uint8_t*)resp.getData(), length);
}

void ZigBee::handleTxStatus(ZBTxStatusResponse& resp) {
    Serial.print("TX Status: ");
    Serial.println(resp.getDeliveryStatus(), HEX);
}

void ZigBee::handleATResponse(AtCommandResponse& resp) {
    Serial.print("AT Response ");
    uint8_t* cmd = resp.getCommand();
    Serial.print((char)cmd[0]);
    Serial.print((char)cmd[1]);
    Serial.print(" status=0x");
    Serial.println(resp.getStatus(), HEX);
}

void ZigBee::handleRemoteATResponse(RemoteAtCommandResponse& resp) {
    Serial.println("Remote AT Response");
    Serial.print("Status: ");
    Serial.println(resp.getStatus(), HEX);
}

void ZigBee::handleModemStatus(ModemStatusResponse& resp) {
    Serial.print("Modem status: ");
    Serial.println(resp.getStatus(), HEX);
}

void ZigBee::handleUnknownPacket() {
    Serial.println("Unknown packet type received");
}

// ---------------------------------------------------------------------------
// public ZigBee API
// ---------------------------------------------------------------------------

void ZigBee::setup() {
    serial.begin(ZIGBEE_SERIAL_BAUD);
    xbee.setSerial(serial);
    xbee.begin(serial);

    // configure a few default AT parameters
    startParamSession();
    setParam("AP", ZIGBEE_API_ENABLED);
    setParam("CE", ZIGBEE_COORDINATOR);
    setParam("ID", ZIGBEE_PAN_ID);
    setParam("SC", ZIGBEE_CHANNELS);
    endParamSession();

    // Serial.println("ZigBee setup complete");
}

void ZigBee::loop(int index) {
    xbee.readPacket();
    if (xbee.getResponse().isAvailable()) {
        processPacket();
    }
}

void ZigBee::send(const XBeeAddress64& address, uint8_t* payload, uint8_t length) {
    ZBTxRequest tx(address, payload, length);
    xbee.send(tx);
}

void ZigBee::send(const XBeeAddress64& address, const char* payload) {
    send(address, (uint8_t*)payload, strlen(payload));
}

void ZigBee::send(uint8_t* payload, uint8_t length) {
    send(COORDINATOR_ADDR, payload, length);
}

void ZigBee::send(const char* payload) {
    send((uint8_t*)payload, strlen(payload));
}

void ZigBee::broadcast(uint8_t* payload, uint8_t length) {
    XBeeAddress64 bcast(0x00000000, BROADCAST_ADDRESS);
    ZBTxRequest tx(bcast, payload, length);
    xbee.send(tx);
}

void ZigBee::broadcast(const char* payload) {
    broadcast((uint8_t*)payload, strlen(payload));
}

// ---------------------------------------------------------------------------
// Cloud implementation
// ---------------------------------------------------------------------------

void Cloud::setup() {
    subscriptionCount = 0;
}

void Cloud::loop(int index) {
    // nothing to do for now
}

void Cloud::subscribe(const char* topic, Callback callback) {
    if (subscriptionCount < sizeof(subscriptions) / sizeof(subscriptions[0])) {
        subscriptions[subscriptionCount++] = { topic, callback };
    }
}

void Cloud::publish(const char* topic, const char* payload) {
    // simple encoding: topic:payload
    size_t tlen = strlen(topic);
    size_t plen = strlen(payload);
    char buf[128];
    if (tlen + plen + 1 >= sizeof(buf)) return;
    memcpy(buf, topic, tlen);
    buf[tlen] = ':';
    memcpy(buf + tlen + 1, payload, plen);
    buf[tlen + 1 + plen] = '\0';
    ZigBee::broadcast(buf);
}

void Cloud::deliver(XBeeAddress64& sender, uint8_t* payload, uint8_t length) {
    // parse "topic:rest"
    int colon = -1;
    for (uint8_t i = 0; i < length; i++) {
        if (payload[i] == ':') { colon = i; break; }
    }
    if (colon < 0) return;
    String topic = String((char*)payload).substring(0, colon);
    uint8_t* msg = payload + colon + 1;
    uint8_t msgLen = length - colon - 1;

    for (uint8_t i = 0; i < subscriptionCount; i++) {
        if (topic == subscriptions[i].topic) {
            subscriptions[i].cb(sender, msg, msgLen);
        }
    }
}
