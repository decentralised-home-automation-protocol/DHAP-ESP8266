#include "IoTDevice.h"

class DeviceStatus : public Status
{
public:
  int stepper = 0;
  int rangeInput = 1000;
  int selection = 0;
  int statusValue = 0;
  int progress = 0;

  boolean buttons = true;

  String getStatus()
  {
    char status[120];
    if (buttons)
    {
      sprintf(status, "true,true,%d,%d,%d,%d,updated%d,%d,1!12:30,password", stepper, rangeInput, selection, statusValue, progress, progress);
    }
    else
    {
      sprintf(status, "false,false,%d,%d,%d,%d,updated%d,%d,2!11:50,password1", stepper, rangeInput, selection, statusValue, progress, progress);
    }
    buttons = !buttons;
    stepper++;
    rangeInput++;
    selection++;
    if (selection > 5)
    {
      selection = 0;
    }
    statusValue++;
    progress++;
    if (progress > 100)
    {
      progress = 0;
    }
    return status;
  }

  void executeCommand(String elementId, String data)
  {
    Serial.printf("IotCommand: id: %s data: %s\n", elementId.c_str(), data.c_str());
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
