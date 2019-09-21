#include "IoTDevice.h"

class DeviceStatus : public Status
{
public:
  bool online = true;
  int currentTemp = 15;
  int targetTemp = 23;
  int fanSpeed = 5;
  int mode = 2;
  char time[9] = "10:00 AM";

  char *trueString = "true";
  char *falseString = "false";

  boolean buttons = true;

  String getStatus()
  {
    char status[120];

    sprintf(status, "%s,%d,%d,%d,%d!%s", online ? trueString : falseString, currentTemp, targetTemp, fanSpeed, mode, time);

    return status;
  }

  void executeCommand(char *command)
  {
    String id = getCommandId(command);
    String data = getCommandData(command);

    Serial.printf("IotCommand: id: %s data: %s\n", id.c_str(), data.c_str());

    if (!strcmp(id.c_str(), "1-1"))
    {
      online = !online;
    }
    else if (!strcmp(id.c_str(), "2-2"))
    {
      targetTemp = atoi(data.c_str());
    }
    else if (!strcmp(id.c_str(), "3-1"))
    {
      fanSpeed = atoi(data.c_str());
    }
    else if (!strcmp(id.c_str(), "4-1"))
    {
      char schedulerData[12];
      strcpy(schedulerData, data.c_str());
      mode = atoi(strtok(schedulerData, "!"));

      char *timeString = strtok(NULL, "!");
      if (timeString == NULL)
      {
        return;
      }

      strcpy(time, timeString);
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
  if (ioTDevice.commandReceived(iotCommand))
  {
    deviceStatus->executeCommand(iotCommand);
  }
}
