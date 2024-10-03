#ifndef ENOCEAN_H
#define ENOCEAN_H

#include <Arduino.h>
#include "CRC8.h"

#define ENOCEAN_HEADER 4
#define ENOCEAN_MAX_DATA 40

class Packet {
public:
    Packet();
    byte senderAddress[4];
    String state;
private:
    void handleTelegram();
    void handleRPSTelegram();
    bool checkHeaderCRC8();
    bool checkDataCRC8();
    void getPacketLength();
    byte header[ENOCEAN_HEADER];
    byte payload[ENOCEAN_MAX_DATA];
    byte enocean_data[ENOCEAN_MAX_DATA];
    byte enocean_optional[ENOCEAN_MAX_DATA];
    uint8_t optionalLength;
    uint8_t dataLength;
    uint8_t payloadLength;
    byte rssi;
    byte type;
    CRC8 crc8{};
    String radioTelegramType;
    byte dataBytes[4];

};

void setupEnocean(int rxPin, int txPin);

#endif // ENOCEAN_H