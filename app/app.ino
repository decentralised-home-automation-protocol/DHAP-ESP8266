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
        char str[80];
        if(buttons){
          sprintf(str, "1-1=true,2-1=open,3-1=%d,4-1=%d,6-1=%d,7-1=%d,8-1=updated1,9-1=%d,11-1=1!12:30,12-1=password", stepper, rangeInput, selection, statusValue,progress);
//          sprintf(str, "1-1=true,2-1=open,3-1=%d,4-1=%d,6-1=%d,7-1=%d,8-1=updated1,9-1=%d,11-1=1!12:30,12-1=password", stepper, rangeInput, selection, statusValue, progress);
        }else{
          sprintf(str, "1-1=true,2-1=open,3-1=%d,4-1=%d,6-1=%d,7-1=%d,8-1=updated1,9-1=%d,11-1=1!12:30,12-1=password", stepper, rangeInput, selection, statusValue,progress);

//          sprintf(str, "1-1=false,2-1=close,3-1=%d,4-1=%d,6-1=%d,7-1=%d,8-1=updated2,9-1=%d,11-1=1!11:40,12-1=password2", stepper, rangeInput, selection, statusValue, progress);
        }
        buttons = !buttons;
        stepper++;
        rangeInput++;
        selection++;
        if (selection > 5) {
          selection = 0;
        }
        statusValue++;
        progress++;
        if (progress > 100) {
          progress = 0;
        }
        return str;
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
