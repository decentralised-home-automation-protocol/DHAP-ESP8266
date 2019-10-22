#include "IoTDevice.h"

class DeviceStatus : public Status
{
public:
  boolean on = false;

  String getStatus()
  {
    char status[120];
    if (on)
    {
      sprintf(status, "true");
    }
    else
    {
      sprintf(status, "false");
    }
    return status;
  }

  void executeCommand(String elementId, String data)
  {
    Serial.printf("IotCommand: id: %s data: %s\n", elementId.c_str(), data.c_str());
    on = !on;

    if(on){
      Serial.printf("Setting high\n");
      digitalWrite(D3,HIGH);
    }else{
      Serial.printf("Setting low\n");
      digitalWrite(D3,LOW);
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

  pinMode(D3,OUTPUT);
//  digitalWrite(D3,LOW);

  ioTDevice.setup(deviceStatus, false);
}

void loop()
{
  ioTDevice.loop();
}
