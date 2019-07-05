#include "iot.h"

iot iotManager;

char iotCommand[255];

void setup()
{
    Serial.begin(115200);
    Serial.println();

    iotManager.setup(false);
}

void loop()
{
    if (iotManager.commandRecieved(iotCommand))
    {
        Serial.printf("IotCommand: %s\n", iotCommand);
    }
}