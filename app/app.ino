#include "IoTDevice.h"

class DeviceStatus : public Status
{
public:
  bool online = true;
  int currentTemp = 15;
  int targetTemp = 23;
  int fanSpeed = 5;
  int mode = 2;
  char time0[6] = "10:00";
  char time1[6] = "14:00";
  char time2[6] = "17:00";
  char time3[6] = "09:50";

  char *trueString = "true";
  char *falseString = "false";

  boolean buttons = true;

  String getStatus()
  {
    char status[120];

    if(mode == 0){
      sprintf(status, "%s,%d,%d,%d,%d!%s", online ? trueString : falseString, currentTemp, targetTemp, fanSpeed, mode, time0);
    }else if(mode == 1) {
      sprintf(status, "%s,%d,%d,%d,%d!%s", online ? trueString : falseString, currentTemp, targetTemp, fanSpeed, mode, time1);
    }else if(mode == 2) {
      sprintf(status, "%s,%d,%d,%d,%d!%s", online ? trueString : falseString, currentTemp, targetTemp, fanSpeed, mode, time2);
    }else {
      sprintf(status, "%s,%d,%d,%d,%d!%s", online ? trueString : falseString, currentTemp, targetTemp, fanSpeed, mode, time3);
    }

    return status;
  }

  void executeCommand(String elementId, String data)
  {
    Serial.printf("IotCommand: id: %s data: %s\n", elementId.c_str(), data.c_str());

    if (!strcmp(elementId.c_str(), "1-1"))
    {
      online = !online;
    }
    else if (!strcmp(elementId.c_str(), "2-2"))
    {
      targetTemp = atoi(data.c_str());
    }
    else if (!strcmp(elementId.c_str(), "3-1"))
    {
      fanSpeed = atoi(data.c_str());
    }
    else if (!strcmp(elementId.c_str(), "4-1"))
    {
      char schedulerData[12];
      strcpy(schedulerData, data.c_str());
      int newMode = atoi(strtok(schedulerData, "!"));

      if(newMode != mode) {
        mode = newMode;
        return;
      }
      mode = newMode;

      char *timeString = strtok(NULL, "!");
      if (timeString == NULL)
      {
        return;
      }

      if(mode == 0){
        strcpy(time0, timeString);
      }else if(mode == 1) {
        strcpy(time1, timeString);
      }else if(mode == 2) {
        strcpy(time2, timeString);
      }else {
        strcpy(time3, timeString);
      }
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
