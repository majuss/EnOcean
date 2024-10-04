#ifndef ENOCEAN_H
#define ENOCEAN_H

#include <Arduino.h>
#include "CRC8.h"

#define ENOCEAN_HEADER 4
#define ENOCEAN_MAX_DATA 40

class Packet {
public:
    Packet();
    bool checkHeaderCRC8(byte CRC8H);
    void handleTelegram();
    bool checkDataCRC8(byte CRC8D);
    byte* getHeader();
    String getState();
    byte* getSenderAddress();
    uint8_t payloadLength;
    byte enocean_data[ENOCEAN_MAX_DATA];
    byte enocean_optional[ENOCEAN_MAX_DATA];
    uint8_t optionalLength;
    uint8_t dataLength;
private:
    byte senderAddress[4];
    String state;
    byte header[ENOCEAN_HEADER];
    void handleRPSTelegram();
    void getPacketLength();
    byte payload[ENOCEAN_MAX_DATA];
    byte rssi;
    byte type;
    CRC8 crc8{};
    String radioTelegramType;
    byte dataBytes[4];

};


#endif // ENOCEAN_H