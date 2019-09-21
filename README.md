# esp8266 DHAP

An esp8266 implementation of the [Decentralised Home Automation Protocol]()

This repo represents an IoT device in a smart home. Multiple branches exist in this repo for various example IoT devices such as a smart TV or security camera.

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

## Installation

Download and install the [Arduino IDE](https://www.arduino.cc/en/main/software).

Download and install the [Arduino ESP8266 filesystem uploader plugin](https://github.com/esp8266/arduino-esp8266fs-plugin) for the Arduino IDE.


## Usage

Create a class which inherits from ``` Status ``` such as ``` DeviceStatus ``` in the example implementation found on the master branch. This class will handle the state of your IoT device and will therefore be unique to each device. 

The Status class contains several methods that must be overridden ```getStatus() ```, ```executeCommand() ```, ```getMaxLeaseLength() ``` and ```getMinUpdatePeriod() ```. This class also includes two helper methods ```getCommandId(char *command)``` and  ```getCommandData(char *command)``` which can be used to get the element ID and data from an incoming command.

The implementation of this class is up to you as it will depend on the functionality of your IoT device. For more information on how to structure status updates, the purpose of each of these abstract functions and the DHAP protocol in general, See the [DHAP Documentation](https://decentralised-home-automation-protocol.github.io/DHAP-Documentation/).

``` C
class DeviceStatus : public Status
{
public:
  String getStatus()
  {
    return "My current Status";
  }

  void executeCommand(char* command)
  {
    String id = getCommandId(command);
    String data = getCommandData(command);

    Serial.printf("IotCommand: id: %s data: %s\n", id.c_str(), data.c_str());
  }

  int getMaxLeaseLength()
  {
    return 100000;
  }

  int getMinUpdatePeriod()
  {
    return 250;
  }
};
```

Instantiate a new instance of your new class in the setup() method.

``` C
IoTDevice ioTDevice;
DeviceStatus *deviceStatus;

void setup()
{
  Serial.begin(115200);
  Serial.println();

  deviceStatus = new DeviceStatus();

  ioTDevice.setup(false, *deviceStatus);
}
```

Then in the loop() method, call ioTDevice.commandRecieved() with an array buffer to store any incoming commands.

``` C
char iotCommand[255];

void loop()
{
  if (ioTDevice.commandRecieved(iotCommand))
  {
    deviceStatus->executeCommand(iotCommand);
  }
}
```

And that's it! all other protocols not unique to any specific device will be handled for you such as joining and discovery.

However, you must remember to upload the devices layout xml using the file system uploader plugin into a file named layout.xml onto the esp8266.

## License

This project is licensed under the [MIT License](LICENSE)

## Credits

- **Tyler Steane** - DHAP Protocol work
- **Aiden Garipoli** - Creator / Maintainer
- **Daniel Milner** - Creator / Maintainer