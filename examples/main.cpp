#include "enocean.h"

void setup()
{
    Serial.begin(115200);
    Serial.println("Setup starting");
    Serial2.begin(57600, SERIAL_8N1, 16, 17);

}

void loop()
{

    while (Serial2.available())
    {
        int const b{Serial2.read()};

        if (b == 0x55)
        {
            do
            {
                yield();
            } while (Serial2.available() < ENOCEAN_HEADER + 1);

            Packet packet;

            Serial2.readBytes(packet.getHeader(), ENOCEAN_HEADER);
            byte const CRC8H{Serial2.read()};

            if (packet.checkHeaderCRC8(CRC8H))
            {
                Serial.println("Header CRC8 is valid.");
            }
            else
            {
                Serial.println("Header CRC8 is invalid.");
                return;
            }
            byte waitCounter;
            byte available;
            do
            {
                yield();
                available = Serial2.available();

            } while (available < packet.payloadLength + 1 || waitCounter++ > 50);

            if (packet.payloadLength + 1 <= available)
            {
                Serial.println("read data");

                Serial2.readBytes(packet.enocean_data, packet.dataLength);
                Serial2.readBytes(packet.enocean_optional, packet.optionalLength);
                byte const CRC8D = Serial2.read();

                if (packet.checkDataCRC8(CRC8D))
                {
                    Serial.println("Data CRC8 is valid.");
                }
                else
                {
                    Serial.println("Data CRC8 is invalid.");
                    return;
                };
                packet.handleTelegram();

            }
            Serial.println(packet.getState());

            for (int i = 0; i < 4; i++)
            {
                Serial.print("senderAddress[");
                Serial.print(i);
                Serial.print("]: ");
                Serial.println(packet.getSenderAddress()[i], HEX);
            }

        } // magic byte 0x55
    } // while
}