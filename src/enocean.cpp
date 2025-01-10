#include "enocean.h"

union DataLength
{
    byte data[2];
    uint16_t length;
};

Packet::Packet()
{
    // Initialize packet data
    memset(header, 0, ENOCEAN_HEADER);
    memset(payload, 0, ENOCEAN_MAX_DATA);
    memset(dataBytes, 0, sizeof(dataBytes));
    memset(senderAddress, 0, sizeof(senderAddress));
    optionalLength = 0;
    dataLength = 0;
    payloadLength = 0;
    radioTelegramType = "Unknown";
    type = "";
    rssi = 0;
    state = "Unknown";
    // init done

    // Serial2.readBytes(header, ENOCEAN_HEADER);
}

bool Packet::checkHeaderCRC8(byte CRC8H)
{
    getPacketLength();
    // type = header[3];
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

    switch (rorgByte)
    {
    case 0xF6:
        Serial.println("RORG is RPS");
        handleRPSTelegram();
        break;

    case 0xD5:
        radioTelegramType = "1BS"; // 1 Byte communication
        break;

    case 0xA5:
        radioTelegramType = "4BS"; // 4 Byte communication
        break;

    case 0xD2:
        radioTelegramType = "VLD"; // Variable Length Data
        break;

    default:
        Serial.println("Unknown radio telegram type (rorg)");
        break;
    }

    rssi = enocean_optional[5] * -1;
    memcpy(senderAddress, &enocean_data[2], 4);
}

void Packet::handleRPSTelegram()
{
    radioTelegramType = "RPS"; // Repeated Switch Communication
    dataBytes[0] = enocean_data[1];
    Serial.println(dataBytes[0]);

    switch (dataBytes[0])
    {
    case 0xE0: // Windows handles
    case 0xC0:
        type = "window_handle";
        state = "on";
        break;
    case 0x70: // 4-button remote
        type = "four_button_remote_button4";
        state = "on";
        break;
    case 0x50:
        type = "four_button_remote_button3";
        state = "on";
        break;
    case 0x30:
        type = "four_button_remote_button2";
        state = "on";
        break;
    case 0x10:
        type = "four_button_remote_button1";
        state = "on";
        break;
    case 0xF0:
        type = "window_handle";
        state = "off";
        break;
    case 0x00:
        type = "four_button_remote_off";
        state = "off";
        break;
    case 0xD0:
        state = "unknown";
        break;

    default:
        // Handle other cases if needed
        break;
    }
    // Implementation of RPS handling
}

byte *Packet::getHeader()
{
    return header;
}
String Packet::getState()
{
    return state;
}
byte *Packet::getSenderAddress()
{
    return senderAddress;
}

int8_t Packet::getRssi()
{
    return rssi;
}

String Packet::getType()
{
    return type;
}