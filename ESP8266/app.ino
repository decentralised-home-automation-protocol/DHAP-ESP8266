#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include "FS.h"

WiFiUDP Udp;
unsigned int localUdpPort = 4210; // local port to listen on
char incomingPacket[255];         // buffer for incoming packets

char replyPacket[3500]; // a reply string to send back

bool hasJoinedNetwork = false;
bool mounted = false;

IPAddress local_IP(192, 168, 4, 22);
IPAddress gateway(192, 168, 4, 9);
IPAddress subnet(255, 255, 255, 0);

void setup()
{
    Serial.begin(115200);
    Serial.println();

    mounted = SPIFFS.begin();

    joinNetwork("TP-LINK_AE045A", "0358721743");

    // Serial.print("Setting soft-AP configuration ... ");
    // Serial.println(WiFi.softAPConfig(local_IP, gateway, subnet) ? "Ready" : "Failed!");

    // Serial.print("Setting soft-AP ... ");
    // Serial.println(WiFi.softAP("ESPsoftAP_01", "passforap") ? "Ready" : "Failed!");

    // Serial.print("Soft-AP IP address = ");
    // Serial.println(WiFi.softAPIP());

    // Udp.begin(localUdpPort);
    // Serial.printf("Now listening at IP %s, UDP port %d\n", WiFi.softAPIP().toString().c_str(), localUdpPort);
}

void loop()
{

    // openFile();
    // delay(2000);
    performIoTCommands();

    // if (hasJoinedNetwork)
    // {
    //     performIoTCommands();
    // }
    // else
    // {
    //     recieveJoiningPackets();
    // }
}

void performIoTCommands()
{
    int packetSize = Udp.parsePacket();
    if (packetSize)
    {
        // receive incoming UDP packets
        Serial.printf("Received %d bytes from %s, port %d\n", packetSize, Udp.remoteIP().toString().c_str(), Udp.remotePort());
        int len = Udp.read(incomingPacket, 255);
        if (len > 0)
        {
            incomingPacket[len] = 0;
        }
        Serial.printf("UDP packet contents: %s\n", incomingPacket);

        openFile();

        // send back a reply, to the IP address and port we got the packet from
        // Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
        // Udp.write(replyPacket);
        // Udp.endPacket();
    }
}

void openFile()
{
    if (mounted)
    {
        Serial.println("mount success");
    }
    else
    {
        Serial.println("mount failed");
    }

    Dir dir = SPIFFS.openDir("");
    String response = "";

    while (dir.next())
    {
        Serial.println(dir.fileName());
        if (dir.fileSize())
        {
            File f = dir.openFile("r");
            size_t size = f.size();

            std::unique_ptr<char[]> buf(new char[size]);

            while (f.position() < f.size())
            {
                String line = f.readString();
                response += line;
                Serial.println(line);
            }
            f.close();
        }
    }
    Serial.println(response);

    strcpy(replyPacket, response.c_str());

    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Udp.write(replyPacket);
    Udp.endPacket();
}

void recieveJoiningPackets()
{
    int packetSize = Udp.parsePacket();
    if (packetSize)
    {
        // receive incoming UDP packets
        Serial.printf("Received %d bytes from %s, port %d\n", packetSize, Udp.remoteIP().toString().c_str(), Udp.remotePort());
        int len = Udp.read(incomingPacket, 255);
        if (len > 0)
        {
            incomingPacket[len] = 0;
        }
        Serial.printf("UDP packet contents: %s\n", incomingPacket);

        char delim[] = ":";
        char *ssid_string = strtok(incomingPacket, delim);
        char *pass_string = strtok(NULL, delim);

        joinNetwork(ssid_string, pass_string);
    }
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