#include "IoTDevice.h"

class DeviceStatus : public Status
{
public:
  bool online = true;
  int source = 0;
  bool recording = true;
  bool muted = false;
  int channel = 0;
  int volume = 0;

  char *trueString = "true";
  char *falseString = "false";

  boolean buttons = true;

  String getStatus()
  {
    char status[120];

    sprintf(status, "%s,%d,%s,%s,%d,%d", online ? trueString : falseString, source, recording ? trueString : falseString, muted ? trueString : falseString, channel, volume);

    return status;
  }

  void executeCommand(char* command)
  {
    String id = getCommandId(command);
    String data = getCommandData(command);

    Serial.printf("IotCommand: id: %s data: %s\n", id.c_str(), data.c_str());

    if (!strcmp(id.c_str(), "1-1"))
    {
      online = !online;
    }
    else if (!strcmp(id.c_str(), "1-2"))
    {
      source = atoi(data.c_str());
    }
    else if (!strcmp(id.c_str(), "2-1"))
    {
      recording = !recording;
    }
    else if (!strcmp(id.c_str(), "2-2"))
    {
      muted = !muted;
    }
    else if (!strcmp(id.c_str(), "3-1"))
    {
      channel = atoi(data.c_str());
    }
    else if (!strcmp(id.c_str(), "4-1"))
    {
      volume = atoi(data.c_str());
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
