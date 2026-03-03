#include <lib/zigBee.h>

// definitions of static members
SoftwareSerial ZigBee::serial(ZIGBEE_RX_PIN, ZIGBEE_TX_PIN);
XBee ZigBee::xbee;

// Cloud static data
Cloud::Subscription Cloud::subscriptions[10];
uint8_t Cloud::subscriptionCount = 0;

// ---------------------------------------------------------------------------
// ZigBee private helpers
// ---------------------------------------------------------------------------

void ZigBee::sendATCommand(const char* command,
    const uint8_t* parameter,
    uint8_t paramLen) {
    // library API expects non-const pointers
    AtCommandRequest req((uint8_t*)command, (uint8_t*)parameter, paramLen);
    xbee.send(req);
    // consume response if available (optional)
    xbee.readPacket(500);
    if (xbee.getResponse().getApiId() == AT_COMMAND_RESPONSE) {
        AtCommandResponse resp;
        xbee.getResponse().getAtCommandResponse(resp);
        Serial.print("AT ");
        Serial.print(command);
        Serial.print(" status=0x");
        Serial.println(resp.getStatus(), HEX);
    }
}

void ZigBee::dumpSettings(const char* settings[]) {
    Serial.println("Current XBee Settings:");
    for (size_t i = 0; settings[i] != nullptr; ++i) {
        sendATCommand(settings[i]);
        delay(100);
        // no further parsing here, sendATCommand already printed status
    }
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
    Serial.print("RX Packet Received, ");
    XBeeAddress64 addr = resp.getRemoteAddress64();
    uint8_t length = resp.getDataLength();
    Serial.print("From: 0x");
    Serial.print((unsigned long)addr.getMsb(), HEX);
    Serial.print(", Payload: ");
    for (uint8_t i = 0; i < length; i++) {
        Serial.print((char)resp.getData()[i]);
    }
    Serial.println();

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
    // sendATCommand("AP", (uint8_t*)ZIGBEE_API_ENABLED, strlen(ZIGBEE_API_ENABLED));
    // sendATCommand("CE", (uint8_t*)ZIGBEE_COORDINATOR, strlen(ZIGBEE_COORDINATOR));
    // sendATCommand("ID", (uint8_t*)ZIGBEE_PAN_ID, strlen(ZIGBEE_PAN_ID));
    // sendATCommand("SC", (uint8_t*)ZIGBEE_CHANNELS, strlen(ZIGBEE_CHANNELS));



    Serial.println("Mode +++");
    serial.print("+++");           // Passage en mode AT
    delay(1000);                       // Attendre OK
    while (serial.available()) {
        Serial.write(serial.read());
    }

    Serial.println("Modification ID");
    serial.print("ATID");
    serial.println(ZIGBEE_PAN_ID);
    delay(100);
    while (serial.available()) {
        Serial.write(serial.read());
    }
    Serial.println();

    Serial.println("Modification CE");
    serial.print("ATCE");
    serial.println(ZIGBEE_COORDINATOR);
    delay(100);
    while (serial.available()) {
        Serial.write(serial.read());
    }
    Serial.println();

    Serial.println("Modification AP");
    serial.print("ATAP");
    serial.println(ZIGBEE_API_ENABLED);
    delay(100);
    while (serial.available()) {
        Serial.write(serial.read());
    }
    Serial.println();

    Serial.println("Modification SC");
    serial.print("ATSC");
    serial.println(ZIGBEE_CHANNELS);
    delay(100);
    while (serial.available()) {
        Serial.write(serial.read());
    }
    Serial.println();

    serial.println("ATCN");  // Quitter le mode AT
    delay(100);

    delay(200);
    Serial.println("ZigBee setup complete");

    delay(200);
    Serial.println("ZigBee setup complete");

    const char* settings[] = { "AP","CE","ID","SC", nullptr };
    dumpSettings(settings);
}

void ZigBee::loop(int index) {
    xbee.readPacket();
    if (xbee.getResponse().isAvailable()) {
        processPacket();
    }

    if (index == 0) {
        send("Hello from cordinator!");
        Serial.println("Sent: Hello from cordinator!");
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

void Cloud::deliver(const XBeeAddress64& sender, uint8_t* payload, uint8_t length) {
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
