#include "IoTDevice.h"

class DeviceStatus : public Status
{
public:
  bool online = true;
  int source = 0;
  bool recording = true;

  char *trueString = "true";
  char *falseString = "false";

  boolean buttons = true;

  String getStatus()
  {
    char status[120];

    sprintf(status, "%s,%d,%s", online ? trueString : falseString, source, recording ? trueString : falseString);

    return status;
  }

  void executeCommand(String elementId, String data)
  {
    Serial.printf("IotCommand: id: %s data: %s\n", elementId.c_str(), data.c_str());

    if (!strcmp(elementId.c_str(), "1-1"))
    {
      online = !online;
    }
    else if (!strcmp(elementId.c_str(), "1-2"))
    {
      source = atoi(data.c_str());
    }
    else if (!strcmp(elementId.c_str(), "2-1"))
    {
      recording = !recording;
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
DeviceStatus deviceStatus;

void setup()
{
  Serial.begin(115200);
  Serial.println();

  ioTDevice.setup(deviceStatus, false);
}

void loop()
{
  ioTDevice.loop();
}
