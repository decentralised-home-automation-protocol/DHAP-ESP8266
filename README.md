# esp8266 DHAP

An esp8266 implementation of the [Decentralised Home Automation Protocol](https://decentralised-home-automation-protocol.github.io/DHAP-Documentation/)

This repo contains an Arduino project which represents an IoT device in a smart home which uses the DHAP protocol. Multiple branches exist in this repo for various example IoT devices such as a smart TV or security camera.

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

## Installation

 - Download and install the [Arduino IDE](https://www.arduino.cc/en/main/software).

 - Download and install the [Arduino ESP8266 filesystem uploader plugin](https://github.com/esp8266/arduino-esp8266fs-plugin) for the Arduino IDE. This plugin is required to upload the device xml file onto the ESP8266 device. The xml must be stored in the file `app/data/layout.xml`.

 - Clone the repo and open it in the Arduino IDE. The only file that you should need to edit is `app.ino`.

## Usage

Inside the file `app.ino`, the class called `DeviceStatus` which inherits from `Status` is found. This should be the only class where you will add your device specific code. This class will handle the state of your IoT device and will therefore be unique to each device. As an overview, the Status class contains several methods that must be overridden `getStatus()`, `executeCommand(String elementID, String data)`, `getMaxLeaseLength()` and `getMinUpdatePeriod()`. 

The implementation of these methods is up to you as it will depend on the functionality of your IoT device. For more information on how to structure status updates as well as the structure of IoT commands, see [the status update packet](https://decentralised-home-automation-protocol.github.io/DHAP-Documentation/reference/packet-types.html#status-update) and [the command packet](https://decentralised-home-automation-protocol.github.io/DHAP-Documentation/reference/packet-types.html#command-request). This documentation describes the structure of the various network packets sent to and from you IoT device. 

More detailed usage instructions can be found in the [DHAP documentation](https://decentralised-home-automation-protocol.github.io/DHAP-Documentation/guide/esp8266.html)

## License

This project is licensed under the [MIT License](LICENSE)

## Credits

- **Tyler Steane** - DHAP Protocol work
- **Aiden Garipoli** - Creator / Maintainer
- **Daniel Milner** - Creator / Maintainer