#include "IoTDevice.h"

IoTDevice ioTDevice;

char iotCommand[255];

void setup()
{
    Serial.begin(115200);
    Serial.println();

    ioTDevice.setup(false);
}

void loop()
{
    if (ioTDevice.commandRecieved(iotCommand))
    {
        Serial.printf("IotCommand: %s\n", iotCommand);
    }
}