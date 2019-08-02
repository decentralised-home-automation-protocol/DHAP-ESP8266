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
    const int PACKET_TYPE_HEADER_LENGTH = 4;

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
        WiFi.mode(WIFI_AP_STA);

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

    bool setupWiFi()
    {
        WiFi.mode(WIFI_STA);

        int timeout = 30;
        Serial.printf("Attempting to connect...\n");

        while (WiFi.status() != WL_CONNECTED && timeout > 0)
        {
            delay(1000);
            Serial.printf("Timeout in...%d\n", timeout);
            timeout--;
        }

        if (timeout > 0)
        {
            Serial.println("WiFi connected");
            Serial.println("IP address: ");
            Serial.println(WiFi.localIP());
            hasJoinedNetwork = true;
        }
        else
        {
            Serial.println("Failed to join a known network");
            hasJoinedNetwork = false;
        }

        Udp.begin(localUdpPort);
        return hasJoinedNetwork;
    }

    String getLocalIP()
    {
        return WiFi.localIP().toString();
    }

    String getMacAddress()
    {
        return WiFi.macAddress();
    }

    void sendReplyPacket(String response)
    {
        Serial.printf("Sending packet to...");
        Serial.println(Udp.remoteIP());
        Udp.beginPacket(Udp.remoteIP(), localUdpPort);
        Udp.write(response.c_str());
        Udp.endPacket();
        delay(200);
    }

    bool joinNetwork(char *SSID, char *password)
    {
        WiFi.begin(SSID, password);

        int timeout = 30;
        Serial.printf("Attempting to connect...\n");
        Serial.printf("SSID: %s\n", SSID);
        Serial.printf("password: %s\n", password);

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

        sendReplyPacket("120");

        WiFi.mode(WIFI_STA);

        Serial.println("WiFi connected");
        Serial.println("IP address: ");
        Serial.println(WiFi.localIP());

        hasJoinedNetwork = true;
        return hasJoinedNetwork;
    }

    void broadcastStatus(String status)
    {
        Serial.printf("Broadcasting: ");
        Serial.println(status);
        Udp.beginPacket(broadcast, localUdpPort);
        Udp.write(status.c_str());
        Udp.endPacket();
        delay(200);
    }

    void discovery()
    {
        char reply[32];

        if (strcmp(incomingPacket, "300") != 0)
        {
            // check if this device is on the list
            char *end_str;
            char *device = strtok_r(incomingPacket + PACKET_TYPE_HEADER_LENGTH, "-", &end_str);

            while (device != NULL)
            {
                char *end_token;
                char *MACAddress = strtok_r(device, ",", &end_token);
                char *statusBit = strtok_r(NULL, ",", &end_token);
                char *visibilityBit = strtok_r(NULL, ",", &end_token);

                Serial.printf("%s <=> %s\n", MACAddress, WiFi.macAddress().c_str());

                if (strcmp(MACAddress, WiFi.macAddress().c_str()) == 0)
                {
                    // this device is on the list
                    Serial.println("This device is on the census list");
                    return;
                    // TODO: check the device details are up to date.
                }

                device = strtok_r(NULL, "-", &end_str);
            }
            Serial.print("Device not in census list\n");
        }
        else
        {
            Serial.print("Census list is empty\n");
        }

        sprintf(reply, "310|%s,%d,%d", WiFi.macAddress().c_str(), 0, 0);
        Serial.print("Reply packet: ");
        Serial.println(reply);

        randomSeed(WiFi.macAddress()[5]);
        delay(random(0, 200));
        sendReplyPacket(reply);
    }

    int getRecentPacketType()
    {
        char packet[255];
        strcpy(packet, incomingPacket);
        char *type = strtok(packet, "|");
        return atoi(type);
    }
};