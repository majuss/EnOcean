#include "enocean.h"

void setup()
{
    Serial.begin(115200);
    setupEnocean(16, 17); // for ESP32
}

void loop()
{

    while (Serial2.available())
    {
        byte const b{Serial2.read()};

        if (b == 0x55)
        {
            do
            {
                yield();
            } while (Serial2.available() < ENOCEAN_HEADER + 1);

            Packet packet;
            Serial.println(packet.state);
            for (int i = 0; i < 4; i++)
            {
                Serial.print("senderAddress[");
                Serial.print(i);
                Serial.print("]: ");
                Serial.println(packet.senderAddress[i], HEX);
            }

        } // magic byte 0x55
    } // while
}