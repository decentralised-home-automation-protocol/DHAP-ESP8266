#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include "FS.h"

IPAddress local_IP(192, 168, 4, 22);
IPAddress gateway(192, 168, 4, 9);
IPAddress subnet(255, 255, 255, 0);

class iot
{
public:
    WiFiUDP Udp;
    unsigned int localUdpPort = 4210; // local port to listen on
    char incomingPacket[255];         // buffer for incoming packets
    char *replyPacket;                // a reply string to send back
    bool hasJoinedNetwork = false;

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
            setupAccessPoint();
        }
        else
        {
            joinNetwork("TP-LINK_AE045A", "0358721743");
        }
    }

    void setupAccessPoint()
    {
        Serial.print("Setting soft-AP configuration ... ");
        Serial.println(WiFi.softAPConfig(local_IP, gateway, subnet) ? "Ready" : "Failed!");

        Serial.print("Setting soft-AP ... ");
        Serial.println(WiFi.softAP("ESPsoftAP_01", "passforap") ? "Ready" : "Failed!");

        Serial.print("Soft-AP IP address = ");
        Serial.println(WiFi.softAPIP());

        Udp.begin(localUdpPort);
        Serial.printf("Now listening at IP %s, UDP port %d\n", WiFi.softAPIP().toString().c_str(), localUdpPort);
    }

    bool commandRecieved(char *iotCommand)
    {
        int packetSize = Udp.parsePacket();
        if (packetSize)
        {
            Serial.printf("Received %d bytes from %s, port %d\n", packetSize, Udp.remoteIP().toString().c_str(), Udp.remotePort());
            int len = Udp.read(incomingPacket, 255);
            if (len > 0)
            {
                incomingPacket[len] = 0;
            }
            Serial.printf("UDP packet contents: %s\n", incomingPacket);
            return handleIncomingPacket(iotCommand);
        }
        return false;
    }

    bool handleIncomingPacket(char *iotCommand)
    {
        if (hasJoinedNetwork)
        {
            if (isUiRequest())
            {
                sendXMLfile();
            }
            else
            {
                strcpy(iotCommand, incomingPacket);
                return true;
            }
        }
        else
        {
            recieveJoiningPackets();
        }
        return false;
    }

    bool isUiRequest()
    {
        return incomingPacket[0] == 'U' && incomingPacket[1] == 'I';
    }

    void sendXMLfile()
    {
        readFile();

        Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
        Udp.write(replyPacket);
        Udp.endPacket();
    }

    void readFile()
    {
        String response;
        File file = SPIFFS.open("/TV.xml", "r");
        replyPacket = new char[file.size()];

        while (file.position() < file.size())
        {
            response += file.readString();
        }
        file.close();

        strcpy(replyPacket, response.c_str());
    }

    void recieveJoiningPackets()
    {
        char delim[] = ":";
        char *ssid_string = strtok(incomingPacket, delim);
        char *pass_string = strtok(NULL, delim);

        joinNetwork(ssid_string, pass_string);
    }

    void joinNetwork(char *SSID, char *password)
    {
        Serial.printf("Joining network\nSSID: %s\nPassword: %s", SSID, password);

        WiFi.mode(WIFI_STA);
        WiFi.begin(SSID, password);

        while (WiFi.status() != WL_CONNECTED)
        {
            delay(500);
            Serial.print(".");
        }

        Serial.println("WiFi connected");
        Serial.println("IP address: ");
        Serial.println(WiFi.localIP());
        Udp.begin(localUdpPort);

        hasJoinedNetwork = true;
    }
};