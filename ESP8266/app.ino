#include "IoTDevice.h"

class DeviceStatus : public Status
{
public:
    String getStatus()
    {
        return "All good.";
    }

    virtual int getMaxLeaseLength()
    {
        //20 minutes maximum lease length.
        return 200000;
    }

    virtual int getMinUpdatePeriod()
    {
        //250ms minimum update period.
        return 250;
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

    ioTDevice.setup(false, *deviceStatus);
}

void loop()
{
    if (ioTDevice.commandRecieved(iotCommand))
    {
        Serial.printf("IotCommand: %s\n", iotCommand);
    }
}