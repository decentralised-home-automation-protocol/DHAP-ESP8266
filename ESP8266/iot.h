#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include "FS.h"
#include "network.h"

class iot
{
public:
    WiFiUDP Udp;
    network net;
    char recentPacket[255]; // buffer for incoming packets

    char *DEFAULT_SSID = "TP-LINK_AE045A";
    char *DEFAULT_PASSWORD = "0358721743";

    void setup(bool setupAP)
    {
        if (SPIFFS.begin())
        {
            Serial.println("mount success");
        }
        else
        {
            Serial.println("mount failed");
        }

        if (setupAP)
        {
            net.setupAccessPoint();
        }
        else
        {
            net.joinNetwork(DEFAULT_SSID, DEFAULT_PASSWORD);
        }
    }

    bool commandRecieved(char *iotCommand)
    {
        bool newIncomingCommandReceived = net.commandRecieved();
        if (newIncomingCommandReceived)
        {
            return handleIncomingPacket(iotCommand);
        }
        else
        {
            return false;
        }
    }

    bool handleIncomingPacket(char *iotCommand)
    {
        if (net.hasJoinedNetwork)
        {
            if (isCommandUiRequest())
            {
                net.sendXMLfile();
            }
            else
            {
                net.getIoTCommand(iotCommand);
                return true;
            }
        }
        else
        {
            char *SSID;
            char *password;
            getSSIDandPasswordFromCommand(SSID, password);
            net.joinNetwork(SSID, password);
        }
        return false;
    }

    bool isCommandUiRequest()
    {
        net.getRecentPacket(recentPacket);
        return recentPacket[0] == 'U' && recentPacket[1] == 'I';
    }

    void getSSIDandPasswordFromCommand(char *SSID, char *password)
    {
        net.getRecentPacket(recentPacket);

        char delim[] = ":";
        SSID = strtok(recentPacket, delim);
        password = strtok(NULL, delim);
    }
};
