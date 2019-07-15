#include "IoTDevice.h"

class DeviceStatus : public Status
{
public:
    String getStatus()
    {
        return "All good.";
    }
};

IoTDevice ioTDevice;
DeviceStatus *deviceStatus;
char iotCommand[255];

void setup()
{
    Serial.begin(115200);
    Serial.println();

    deviceStatus = new DeviceStatus();

    ioTDevice.setup(true, *deviceStatus);
}

void loop()
{
    if (ioTDevice.commandRecieved(iotCommand))
    {
        Serial.printf("IotCommand: %s\n", iotCommand);
    }
}