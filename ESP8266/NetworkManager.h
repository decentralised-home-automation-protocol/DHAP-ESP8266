#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

IPAddress local_IP(192, 168, 4, 22);
IPAddress gateway(192, 168, 4, 9);
IPAddress subnet(255, 255, 255, 0);
IPAddress broadcast(255, 255, 255, 255);

class NetworkManager
{
private:
    WiFiUDP Udp;

    unsigned int localUdpPort = 8888; // local port to listen on
    char *DEFAULT_SSID = "ESPsoftAP_01";
    char *DEFAULT_PASSWORD = "passforap";

public:
    char incomingPacket[255]; // buffer for incoming packets
    bool hasJoinedNetwork = false;

    bool newCommandRecieved()
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
            return true;
        }
        return false;
    }

    void getRecentPacket(char *recentPacket)
    {
        strcpy(recentPacket, incomingPacket);
    }

    void setupAccessPoint()
    {
        Serial.print("Setting soft-AP configuration ... ");
        Serial.println(WiFi.softAPConfig(local_IP, gateway, subnet) ? "Ready" : "Failed!");

        Serial.print("Setting soft-AP ... ");
        Serial.println(WiFi.softAP(DEFAULT_SSID, DEFAULT_PASSWORD) ? "Ready" : "Failed!");

        Serial.print("Soft-AP IP address = ");
        Serial.println(WiFi.softAPIP());

        Udp.begin(localUdpPort);
        Serial.printf("Now listening at IP %s, UDP port %d\n", WiFi.softAPIP().toString().c_str(), localUdpPort);
        hasJoinedNetwork = false;
    }

    String getLocalIP()
    {
        return WiFi.localIP().toString();
    }

    void sendReplyPacket(String response)
    {
        Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
        Udp.write(response.c_str());
        Udp.endPacket();
    }

    bool joinNetwork(char *SSID, char *password)
    {
        WiFi.mode(WIFI_AP_STA);
        WiFi.begin(SSID, password);

        int timeout = 30;
        Serial.printf("Attempting to connecting to %s...\n", SSID);

        while (WiFi.status() != WL_CONNECTED)
        {
            delay(1000);
            Serial.printf("Timeout in...%d\n", timeout);
            timeout--;
            if (timeout == 0)
            {
                return false;
            }
        }

        WiFi.mode(WIFI_STA);

        Serial.println("WiFi connected");
        Serial.println("IP address: ");
        Serial.println(WiFi.localIP());
        Udp.begin(localUdpPort);

        hasJoinedNetwork = true;
        return hasJoinedNetwork;
    }

    void broadcastStatus(String status)
    {
        Serial.printf("Broadcasting: ");
        Serial.println(status);
        Udp.beginPacket(broadcast, Udp.remotePort());
        Udp.write(status.c_str());
        Udp.endPacket();
    }
};