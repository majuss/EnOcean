#include "enocean.h"

union DataLength
{
    byte data[2];
    uint16_t length;
};

Packet::Packet()
{
    memset(header, 0, ENOCEAN_HEADER);
    memset(payload, 0, ENOCEAN_MAX_DATA);
    memset(dataBytes, 0, sizeof(dataBytes));
    memset(senderAddress, 0, sizeof(senderAddress));
    optionalLength = 0;
    dataLength = 0;
    payloadLength = 0;
    radioTelegramType = "Unknown";
    type = 0;
    rssi = 0;
    state = "Unknown";
}

bool Packet::checkHeaderCRC8(byte CRC8H)
{
    getPacketLength();
    type = header[3];
    crc8.begin();
    uint8_t checksum = crc8.get_crc8(header, ENOCEAN_HEADER);
    return checksum == CRC8H;
}

bool Packet::checkDataCRC8(byte CRC8D)
{

    byte enocean_buffer[2 * ENOCEAN_MAX_DATA];
    memcpy(enocean_buffer, enocean_data, dataLength);
    memcpy(enocean_buffer + dataLength, enocean_optional, optionalLength);
    crc8.begin();
    uint8_t checksumData = crc8.get_crc8(enocean_buffer, payloadLength);
    rssi = enocean_optional[5];
    return checksumData == CRC8D;
}

void Packet::getPacketLength()
{
    DataLength dl;
    dl.data[0] = header[1];
    dl.data[1] = header[0];
    dataLength = dl.length;
    optionalLength = header[2];

    if (dataLength > ENOCEAN_MAX_DATA || optionalLength > ENOCEAN_MAX_DATA)
    {
        // ERROR length missmatch
    }
    else
    {
        payloadLength = dataLength + optionalLength;
    }
}

void Packet::handleTelegram()
{
    byte rorgByte = enocean_data[0];

    if (rorgByte == 0xF6)
    {
        handleRPSTelegram();
    }
    else if (rorgByte == 0xD5)
    {
        radioTelegramType = "1BS"; // 1 Byte communication
    }
    else if (rorgByte == 0xA5)
    {
        radioTelegramType = "4BS"; // 4 Byte communication
    }
    else if (rorgByte == 0xD2)
    {
        radioTelegramType = "VLD"; // Variable Length Data
    }
    else
    {
        Serial.println("Unknown radio telegram type (rorg)");
    }

    memcpy(senderAddress, &enocean_data[2], 4);

}

void Packet::handleRPSTelegram()
{
    radioTelegramType = "RPS"; // Repeated Switch Communication
    dataBytes[0] = enocean_data[1];

    if (dataBytes[0] == 0xE0 || dataBytes[0] == 0xC0)
    {
        state = "on";
    }
    else if (dataBytes[0] == 0xF0)
    {
        state = "off";
    }
    else
    {
        state = "unknown";
    }

    // Implementation of RPS handling
}

byte* Packet::getHeader()
{
    return header;
}
String Packet::getState()
{
    return state;
}
byte* Packet::getSenderAddress()
{
    return senderAddress;
}
