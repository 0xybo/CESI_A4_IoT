#include <Arduino.h>
#include <SimpleZigBeeRadio.h>

// This sketch exposes a USB-serial command interface to configure
// and use an XBee Series 2 (ZigBee) radio in API mode using the
// SimpleZigBee Arduino library.

// Host serial (USB)
#define HOST Serial

// XBee serial: use Serial1 (hardware UART). If your board doesn't
// provide Serial1, adjust to a SoftwareSerial instance and call
// radio.setSerial(yourSoftSerial).
#define XBEE_SERIAL Serial1

SimpleZigBeeRadio radio(true); // escaped API mode recommended

String lineBuf;

uint16_t cmdStringToUint16(const String& s) {
    if (s.length() < 2) return 0;
    return (uint16_t(s.charAt(0)) << 8) | uint16_t(s.charAt(1));
}

uint32_t hexPartToUint32(const String& hex, int start, int len) {
    uint32_t v = 0;
    for (int i = 0; i < len; ++i) {
        char c = hex.charAt(start + i);
        uint8_t nib = 0;
        if (c >= '0' && c <= '9') nib = c - '0';
        else if (c >= 'A' && c <= 'F') nib = 10 + (c - 'A');
        else if (c >= 'a' && c <= 'f') nib = 10 + (c - 'a');
        v = (v << 4) | nib;
    }
    return v;
}

void printHelp() {
    HOST.println(F("Commands:"));
    HOST.println(F("  help"));
    HOST.println(F("  at <CMD> [hex_payload]         - local AT command"));
    HOST.println(F("  remote <64hex> <CMD> [payload] - remote AT"));
    HOST.println(F("  send <64hex> <text>            - send TX request to addr"));
    HOST.println(F("Examples:"));
    HOST.println(F("  at ID 1234"));
    HOST.println(F("  remote 0013A20040B2ABCD DL 0"));
}

void sendLocalAT(const String& cmd, const String& payloadHex) {
    uint16_t c = cmdStringToUint16(cmd);
    if (payloadHex.length() == 0) {
        radio.prepareATCommand(c);
    }
    else {
        int plen = payloadHex.length() / 2;
        uint8_t buf[plen];
        for (int i = 0; i < plen; ++i) {
            String byteHex = payloadHex.substring(i * 2, i * 2 + 2);
            buf[i] = (uint8_t)strtoul(byteHex.c_str(), NULL, 16);
        }
        radio.prepareATCommand(c, buf, plen);
    }
    radio.send();
}

void sendRemoteAT(const String& addr64, const String& cmd, const String& payloadHex) {
    // addr64 should be 16 hex chars
    if (addr64.length() != 16) {
        HOST.println(F("addr64 must be 16 hex chars"));
        return;
    }
    uint32_t msb = hexPartToUint32(addr64, 0, 8);
    uint32_t lsb = hexPartToUint32(addr64, 8, 8);
    uint16_t c = cmdStringToUint16(cmd);
    if (payloadHex.length() == 0) {
        radio.prepareRemoteATCommand(msb, lsb, 0xFFFE, c);
    }
    else {
        int plen = payloadHex.length() / 2;
        uint8_t buf[plen];
        for (int i = 0; i < plen; ++i) buf[i] = (uint8_t)strtoul(payloadHex.substring(i * 2, i * 2 + 2).c_str(), NULL, 16);
        radio.prepareRemoteATCommand(msb, lsb, 0xFFFE, c, buf, plen);
    }
    radio.send();
}

void sendTX(const String& addr64, const String& text) {
    if (addr64.length() != 16) {
        HOST.println(F("addr64 must be 16 hex chars"));
        return;
    }
    uint32_t msb = hexPartToUint32(addr64, 0, 8);
    uint32_t lsb = hexPartToUint32(addr64, 8, 8);
    int len = text.length();
    uint8_t payload[len];
    for (int i = 0; i < len; ++i) payload[i] = (uint8_t)text.charAt(i);
    radio.prepareTXRequest(msb, lsb, 0xFFFE, payload, len);
    radio.send();
}

void handleHostLine(const String& line) {
    if (line.length() == 0) return;
    // simple tokenization by spaces
    int idx = 0;
    String cmd = "";
    int pos = line.indexOf(' ');
    if (pos == -1) cmd = line;
    else {
        cmd = line.substring(0, pos);
    }
    if (cmd.equalsIgnoreCase("help")) { printHelp(); return; }
    if (cmd.equalsIgnoreCase("at")) {
        // at <CMD> [hex]
        String rest = (pos == -1) ? "" : line.substring(pos + 1);
        int p = rest.indexOf(' ');
        String acmd = (p == -1) ? rest : rest.substring(0, p);
        String payload = (p == -1) ? "" : rest.substring(p + 1);
        sendLocalAT(acmd, payload);
        return;
    }
    if (cmd.equalsIgnoreCase("remote")) {
        // remote <64hex> <CMD> [payload]
        String rest = (pos == -1) ? "" : line.substring(pos + 1);
        int p1 = rest.indexOf(' ');
        if (p1 == -1) { HOST.println(F("usage: remote <64hex> <CMD> [payload]")); return; }
        String addr = rest.substring(0, p1);
        String rest2 = rest.substring(p1 + 1);
        int p2 = rest2.indexOf(' ');
        String rcmd = (p2 == -1) ? rest2 : rest2.substring(0, p2);
        String payload = (p2 == -1) ? "" : rest2.substring(p2 + 1);
        sendRemoteAT(addr, rcmd, payload);
        return;
    }
    if (cmd.equalsIgnoreCase("send")) {
        // send <64hex> <text>
        String rest = (pos == -1) ? "" : line.substring(pos + 1);
        int p = rest.indexOf(' ');
        if (p == -1) { HOST.println(F("usage: send <64hex> <text>")); return; }
        String addr = rest.substring(0, p);
        String text = rest.substring(p + 1);
        sendTX(addr, text);
        return;
    }
    HOST.println(F("Unknown command. Type 'help'."));
}

void setup() {
    HOST.begin(115200);
    // give host time to open serial
    delay(50);
    HOST.println(F("ZigBee USB bridge starting..."));

    // Attach XBee serial
    XBEE_SERIAL.begin(9600);
    radio.setSerial(XBEE_SERIAL);
    radio.setEscaping(true);
    HOST.println(F("XBee serial started (Serial1 @ 9600)."));
    printHelp();
}

void loop() {
    // Read host commands
    while (HOST.available()) {
        char c = HOST.read();
        if (c == '\r') continue;
        if (c == '\n') {
            handleHostLine(lineBuf);
            lineBuf = "";
        }
        else {
            lineBuf += c;
        }
    }

    // Read XBee serial into library
    radio.read();

    // When a frame is available, print a human-readable summary
    if (radio.available()) {
        if (radio.isATResponse()) {
            uint16_t atcmd = radio.getATResponseCommand();
            HOST.print(F("AT Response "));
            char a = (char)(atcmd >> 8); char b = (char)(atcmd & 0xFF);
            HOST.print(a); HOST.print(b); HOST.print(F(" status:"));
            HOST.print(radio.getATResponseStatus());
            HOST.print(F(" payload:"));
            int plen = radio.getATResponsePayloadLength();
            for (int i = 0;i < plen;i++) {
                HOST.print(" "); HOST.print(radio.getATResponsePayload(i), HEX);
            }
            HOST.println();
        }
        else if (radio.isRemoteATResponse()) {
            HOST.print(F("Remote AT Response "));
            uint16_t atcmd = radio.getRemoteATResponseCommand();
            char a = (char)(atcmd >> 8); char b = (char)(atcmd & 0xFF);
            HOST.print(a); HOST.print(b); HOST.print(F(" status:"));
            HOST.print(radio.getRemoteATResponseStatus());
            HOST.print(F(" payload:"));
            int plen = radio.getRemoteATResponsePayloadLength();
            for (int i = 0;i < plen;i++) {
                HOST.print(" "); HOST.print(radio.getRemoteATResponsePayload(i), HEX);
            }
            HOST.println();
        }
        else if (radio.isRX()) {
            HOST.print(F("RX from "));
            SimpleZigBeeAddress64 addr = radio.getRXAddress64();
            HOST.print(addr.getAddressMSB(), HEX); HOST.print(addr.getAddressLSB(), HEX);
            HOST.print(F(" payload:"));
            int plen = radio.getRXPayloadLength();
            for (int i = 0;i < plen;i++) HOST.write(radio.getRXPayload(i));
            HOST.println();
        }
        else if (radio.isTXStatus()) {
            HOST.print(F("TX Status: addr16="));
            HOST.print(radio.getTXStatusAddress16().getAddress(), HEX);
            HOST.print(F(" status="));
            HOST.println(radio.getTXStatusDeliveryStatus(), HEX);
        }
        else if (radio.isModemStatus()) {
            HOST.print(F("Modem Status: ")); HOST.println(radio.getModemStatus(), HEX);
        }
    }
}