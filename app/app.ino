#include "IoTDevice.h"

class DeviceStatus : public Status
{
public:
  bool on = true;
  bool warm = true;
  int temp = 0;
  int setting = 0;

  char *trueString = "true";
  char *falseString = "false";

  boolean buttons = true;

  String getStatus()
  {
    char status[120];

    sprintf(status, "%s,%s,%d,%d", on ? trueString : falseString, warm ? trueString : falseString, temp, setting);

    return status;
  }

  void executeCommand(char *command)
  {
    String id = getCommandId(command);
    String data = getCommandData(command);

    Serial.printf("IotCommand: id: %s data: %s\n", id.c_str(), data.c_str());

    if (!strcmp(id.c_str(), "1-1"))
    {
      on = !on;
    }
    else if (!strcmp(id.c_str(), "3-1"))
    {
      warm = !warm;
    }
    else if (!strcmp(id.c_str(), "4-1"))
    {
      temp = atoi(data.c_str());
    }
    else if (!strcmp(id.c_str(), "5-1"))
    {
      setting = atoi(data.c_str());
    }
  }

  int getMaxLeaseLength()
  {
    //20 minutes maximum lease length.
    return 200000;
  }

  int getMinUpdatePeriod()
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
    deviceStatus->executeCommand(iotCommand);
  }
}
