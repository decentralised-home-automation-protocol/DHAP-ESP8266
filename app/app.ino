#include "IoTDevice.h"

class DeviceStatus : public Status
{
public:
  boolean button[6];
  boolean active = false;

  char *trueString = "true";
  char *falseString = "false";

  String getStatus()
  {
    char status[120];
    if(active) {
      sprintf(status, "%s,%s,%s,%s,%s,%s,%s", active ? trueString : falseString, button[0] ? trueString : falseString, button[1] ? trueString : falseString, button[2] ? trueString : falseString, button[3] ? trueString : falseString, button[4] ? trueString : falseString, button[5] ? trueString : falseString);
    } else {
      sprintf(status, "false,false,false,false,false,false,false");
    }

    return status;
  }

  void executeCommand(String elementId, String data)
  {
    Serial.printf("IotCommand: id: %s data: %s\n", elementId.c_str(), data.c_str());

    if (!strcmp(elementId.c_str(), "1-1"))
    {
      toggle();
    }

    if(!active){
      return;
    }

    if (!strcmp(elementId.c_str(), "2-1"))
    {
      button[0] = !button[0];
      toggleLED(button[0], D2);
    }
    else if (!strcmp(elementId.c_str(), "3-1"))
    {
      button[1] = !button[1];
      toggleLED(button[1], D3);
    }
    else if (!strcmp(elementId.c_str(), "4-1"))
    {
      button[2] = !button[2];
      toggleLED(button[2], D4);
    }
    else if (!strcmp(elementId.c_str(), "5-1"))
    {
      button[3] = !button[3];
      toggleLED(button[3], D5);
    }
    else if (!strcmp(elementId.c_str(), "6-1"))
    {
      button[4] = !button[4];
      toggleLED(button[4], D6);
    }
    else if (!strcmp(elementId.c_str(), "7-1"))
    {
      button[5] = !button[5];
      toggleLED(button[5], D7);
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

  void toggleLED(boolean on, int pin){
    if(!active){
      return;
    }
    if(on){
      digitalWrite(pin, HIGH);
    }else{
      digitalWrite(pin, LOW);
    }
  }

  void toggle(){
    active = !active;
    
    if (active){
      digitalWrite(0, HIGH);
      toggleLED(button[0], D2);
      toggleLED(button[1], D3);
      toggleLED(button[2], D4);
      toggleLED(button[3], D5);
      toggleLED(button[4], D6);
      toggleLED(button[5], D7);      
    } else {
      digitalWrite(0, LOW);
      digitalWrite(D2, LOW);
      digitalWrite(D3, LOW);
      digitalWrite(D4, LOW);
      digitalWrite(D5, LOW);
      digitalWrite(D6, LOW);
      digitalWrite(D7, LOW);
    }
  }
};

IoTDevice ioTDevice;
DeviceStatus deviceStatus;
int val = 0;
int oldVal = 1;

void setup()
{
  Serial.begin(115200);
  Serial.println();

  pinMode(2, INPUT);
  pinMode(0, OUTPUT);

  pinMode(D2, OUTPUT);
  pinMode(D3, OUTPUT);
  pinMode(D4, OUTPUT);
  pinMode(D5, OUTPUT);
  pinMode(D6, OUTPUT);
  pinMode(D7, OUTPUT);

  ioTDevice.setup(deviceStatus, false);
}

void loop()
{
  val = digitalRead(2);
  if(val == 0 && oldVal == 1) {
    deviceStatus.toggle();
  }
  oldVal = val;
  ioTDevice.loop();  
}
