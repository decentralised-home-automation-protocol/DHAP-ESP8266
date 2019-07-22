#include "NetworkManager.h"
#include "FileManager.h"
#include "StatusManager.h"

class IoTDevice
{
private:
    NetworkManager networkManager;
    FileManager fileManager;
    StatusManager statusManager;

    char *ssid;
    char *password;
    char temp[255];

    const int PACKET_TYPE_HEADER_LENGTH = 4;

public:
    void setup(bool forceSetupAP, Status &deviceStatus)
    {
        fileManager.mountFileSystem();

        if (forceSetupAP)
        {
            networkManager.setupAccessPoint();
        }
        else
        {
            if (!networkManager.setupWiFi())
            {
                networkManager.setupAccessPoint();
            }
        }

        statusManager.setStatusController(deviceStatus, networkManager.getMacAddress());
    }

    bool commandRecieved(char *iotCommand)
    {
        String status = statusManager.getStatusUpdateIfNeeded();
        if (status.length() > 0)
        {
            networkManager.broadcastStatus(status);
        }

        if (networkManager.newCommandRecieved())
        {
            return handleIncomingPacket(iotCommand);
        }
        return false;
    }

    bool handleIncomingPacket(char *iotCommand)
    {
        switch (networkManager.getRecentPacketType())
        {
        case 100: //Joining credentials
            joiningPacket();
            return false;
        case 200: //UI Request
            uiRequest();
            return false;
        case 300: //Discovery Request
            discoveryRequest();
            return false;
        case 400: //IoT Command
            getIoTCommand(iotCommand);
            return true;
        case 500: //Status Lease Request
            statusLeaseRequest();
            return false;
        case 520: //Leave Status Lease
            statusManager.removeListeningDevice();
            return false;
        default:
            Serial.println("Unknown Packet");
        };
        return false;
    }

    void joiningPacket()
    {
        if (!networkManager.hasJoinedNetwork)
        {
            networkManager.sendReplyPacket("110");

            //Tokenize Credentials
            networkManager.getRecentPacket(temp);
            char *type = strtok(temp, "|,");
            ssid = strtok(NULL, "|,");
            password = strtok(NULL, "|,");

            if (!networkManager.joinNetwork(ssid, password))
            {
                Serial.println("Failed to join network!");
                networkManager.sendReplyPacket("130");
            }
        }
    }

    void uiRequest()
    {
        Serial.println("UI Request Recieved.");
        String response = fileManager.readFile();
        networkManager.sendReplyPacket("210|" + response);
        Serial.println("XML File sent.");
    }

    void discoveryRequest()
    {
        Serial.println("Discovery Request Recieved.");
        networkManager.discovery();
    }

    void getIoTCommand(char *iotCommand)
    {
        Serial.println("IoT Command Recieved.");
        networkManager.getRecentPacket(temp);
        strcpy(iotCommand, temp + PACKET_TYPE_HEADER_LENGTH);
    }

    void statusLeaseRequest()
    {
        Serial.println("Status Request Recieved. Adding to List...");
        String response = statusManager.newStatusRegistration(networkManager.incomingPacket);
        if (response.length() > 0)
        {
            networkManager.sendReplyPacket(response.c_str());
        }
    }
};