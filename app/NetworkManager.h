#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

IPAddress local_IP(192, 168, 4, 22);
IPAddress gateway(192, 168, 4, 9);
IPAddress subnet(255, 255, 0, 0);
IPAddress broadcast(255, 255, 255, 255);

class NetworkManager
{
private:
    WiFiUDP Udp;
    const int PACKET_TYPE_HEADER_LENGTH = 4;

    unsigned int localUdpPort = 8888; // local port to listen on
    char DEFAULT_SSID[27];
    char *DEFAULT_PASSWORD = "passforap";

    unsigned long previousMillis = 0;
    unsigned long leaseLengthRemaining = 0;
    unsigned long currentMillis = 0;
    unsigned long timeSinceLastUpdate = 0;

public:
    char incomingPacket[255]; // buffer for incoming packets
    char macAddress[18];
    char headerVersion;
    bool hasJoinedNetwork = false;

    void initialise(char *deviceName)
    {
        strcpy(macAddress, WiFi.macAddress().c_str());
        randomSeed(macAddress[6]);
        sprintf(DEFAULT_SSID, "ESP_AP_%s",deviceName);
    }

    bool newCommandReceived()
    {
        int packetSize = Udp.parsePacket();
        if (packetSize)
        {
            int len = Udp.read(incomingPacket, 255);
            if (len > 0)
            {
                incomingPacket[len] = 0;
            }
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
        WiFi.mode(WIFI_AP);

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

    void sendReplyPacket(const char *response)
    {
        Serial.printf("Sending packet to...");
        Serial.println(Udp.remoteIP());

        Udp.beginPacket(Udp.remoteIP(), localUdpPort);
        Udp.write(response);
        Udp.endPacket();
    }

    bool joinNetwork(char *SSID, char *password, bool sendReply)
    {
        WiFi.begin(SSID, password);

        int timeout = 20;
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
                previousMillis = millis();
                broadcastStatus("130");
                return false;
            }

            if (checkForNewJoiningPacket())
            {
                return true;
            }
        }

        if (sendReply)
        {
            sendReplyPacket("120");
            delay(1000);
            sendReplyPacket("120");
            delay(1000);
            sendReplyPacket("120");
        }

        WiFi.mode(WIFI_STA);
        Udp.begin(localUdpPort);

        Serial.println("WiFi connected");
        Serial.println("IP address: ");
        Serial.println(WiFi.localIP());

        hasJoinedNetwork = true;
        return hasJoinedNetwork;
    }

    void broadcastStatus(const char *response)
    {
        Serial.printf("Broadcasting: %s\n", response);

        if (Udp.beginPacket(broadcast, localUdpPort))
        {
            Udp.write(response);
            Udp.endPacket();
            delay(200);
        }
        else
        {
            Serial.println("Error resolving hostname or port");
        }
    }

    void discovery()
    {
        if (strcmp(incomingPacket, "300") != 0)
        {
            // check if this device is on the list
            char *end_str;
            char *device = strtok_r(incomingPacket + PACKET_TYPE_HEADER_LENGTH, "-", &end_str);

            while (device != NULL)
            {
                char *MACAddress = strtok(device, ",");
                char *statusBit = strtok(NULL, ",");
                char *visibilityBit = strtok(NULL, ",");                

                Serial.printf("%s <=> %s\n", macAddress, MACAddress);

                if (strcmp(MACAddress, macAddress) == 0)
                {
                    // this device is on the list
                    Serial.println("This device is on the census list\n");

                    if(!strcmp(statusBit,"0")){
                        Serial.println("Status bit is incorrect. Sending reply...\n");

                        sendDiscoveryReply();
                    }
                    
                    return;
                }

                device = strtok_r(NULL, "-", &end_str);
            }
            Serial.print("Device not in census list\n\n");
        }
        else
        {
            Serial.print("Census list is empty\n");
        }

        sendDiscoveryReply();
    }

    void sendDiscoveryReply(){
        char reply[32];

        sprintf(reply, "310|%s,1,%d,%c", macAddress, 0, headerVersion);
        Serial.print("Reply packet: ");
        Serial.println(reply);

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

    void joinWiFiLoop(char *SSID, char *password)
    {
        //check if its time to send a status update
        currentMillis = millis();
        unsigned long deltaMillis = currentMillis - previousMillis;
        previousMillis = currentMillis;
        timeSinceLastUpdate += deltaMillis;

        if (timeSinceLastUpdate > 240000)
        {
            timeSinceLastUpdate = 0;

            if (SSID == NULL || password == NULL)
            {
                return;
            }
            Serial.println("Attempting to join network again");
            Serial.printf("SSID: %s\n", SSID);
            Serial.printf("Password: %s\n", password);

            joinNetwork(SSID, password, false);
        }
    }

    bool checkForNewJoiningPacket()
    {
        if (newCommandReceived())
        {
            if (getRecentPacketType() == 100)
            {
                char temp[255];
                getRecentPacket(temp);

                char *type = strtok(temp, "|,");
                char *newSsid = strtok(NULL, "|,");
                char *newPassword = strtok(NULL, "|,");

                return joinNetwork(newSsid, newPassword, true);
            }
        }
    }
};
