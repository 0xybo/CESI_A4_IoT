#pragma once

#include <Arduino.h>
#include <SoftwareSerial.h>
#include <XBee.h>
#include <constants.h>

// wrapper around xbee-arduino library to mimic old interface
class ZigBee {
private:
    // use SoftwareSerial so we can free the hardware port for debugging
    static SoftwareSerial serial;
    static XBee xbee;

    // address of the coordinator (all zeroes)
    static constexpr XBeeAddress64 COORDINATOR_ADDR = XBeeAddress64(0x00000000, 0x00000000);

    static void sendATCommand(const char* command,
        const uint8_t* parameter = nullptr,
        uint8_t paramLen = 0);
    static void dumpSettings(const char* settings[]);

    static void processPacket();
    static void handleRxPacket(ZBRxResponse& resp);
    static void handleTxStatus(ZBTxStatusResponse& resp);
    static void handleATResponse(AtCommandResponse& resp);
    static void handleRemoteATResponse(RemoteAtCommandResponse& resp);
    static void handleModemStatus(ModemStatusResponse& resp);
    static void handleUnknownPacket();

public:
    static void setup();
    static void loop(int index);

    // convenience send overrides --- 64‑bit addressing
    static void send(const XBeeAddress64& address, uint8_t* payload, uint8_t length);
    static void send(const XBeeAddress64& address, const char* payload);
    static void send(uint8_t* payload, uint8_t length);           // to coordinator
    static void send(const char* payload);
    static void broadcast(uint8_t* payload, uint8_t length);
    static void broadcast(const char* payload);
};

// tiny pub/sub wrapper that operates on the payload string of incoming
// packets; topics and payloads are seperated by a ':' character.
class Cloud {
public:
    using Callback = void (*)(const XBeeAddress64& sender, uint8_t* payload, uint8_t length);

private:
    struct Subscription {
        const char* topic;
        Callback cb;
    };

    static Subscription subscriptions[10];
    static uint8_t subscriptionCount;

    // called from ZigBee::handleRxPacket when a packet arrives
    friend class ZigBee;
    static void deliver(const XBeeAddress64& sender, uint8_t* payload, uint8_t length);

public:
    static void setup();
    static void loop(int index);

    static void subscribe(const char* topic, Callback callback);
    static void publish(const char* topic, const char* payload);
};