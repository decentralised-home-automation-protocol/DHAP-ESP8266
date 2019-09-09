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
    char header[64];

    const int PACKET_TYPE_HEADER_LENGTH = 4;

public:
    void setup(bool forceSetupAP, Status &deviceStatus)
    {
        fileManager.mountFileSystem();
        networkManager.initialise();

        if (forceSetupAP)
        {
            networkManager.setupAccessPoint();
        }
        else
        {
            char creds[64];
            fileManager.getSavedNetworkCredentials(creds);

            if (strlen(creds) == 0)
            {
                Serial.println("No credentials found!");
                networkManager.setupAccessPoint();
            }
            else
            {
                ssid = strtok(creds, "|,");
                password = strtok(NULL, "|,");

                if (!networkManager.joinNetwork(ssid, password, false))
                {
                    Serial.println("Failed to join network!");
                    networkManager.setupAccessPoint();
                }
            }
        }
        fileManager.getFileHeader(header);
        Serial.printf("File header: %s", header);

        statusManager.setStatusController(deviceStatus, networkManager.macAddress);
    }

    bool commandRecieved(char *iotCommand)
    {
        boolean newStatus = statusManager.getStatusUpdateIfNeeded(temp);
        if (newStatus)
        {
            networkManager.broadcastStatus(temp);
        }

        if (networkManager.newCommandRecieved())
        {
            return handleIncomingPacket(iotCommand);
        }

        if (!networkManager.hasJoinedNetwork)
        {
            networkManager.joinWiFiLoop(ssid, password);
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
        case 320: //Discovery Header Request
            discoveryHeaderRequest();
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
            return false;
        };
        return false;
    }

    void joiningPacket()
    {
        if (!networkManager.hasJoinedNetwork)
        {
            networkManager.sendReplyPacket("110");
            networkManager.sendReplyPacket("110");
            networkManager.sendReplyPacket("110");

            //Tokenize Credentials
            networkManager.getRecentPacket(temp);
            char *type = strtok(temp, "|,");
            ssid = strtok(NULL, "|,");
            password = strtok(NULL, "|,");

            if (!networkManager.joinNetwork(ssid, password, true))
            {
                Serial.println("Failed to join network!");
                networkManager.sendReplyPacket("130");
            }
            else
            {
                fileManager.saveNetworkCredentials(ssid, password);
            }
        }
    }

    void uiRequest()
    {
        Serial.println("UI Request Recieved.");
        int size = fileManager.layoutFileSize + PACKET_TYPE_HEADER_LENGTH;
        char *xml = (char *)malloc(size);
        sprintf(xml, "210|");

        fileManager.readFile(xml + PACKET_TYPE_HEADER_LENGTH);

        //Ensures there isnt any junk after the xml.
        xml[size] = '\0';

        networkManager.sendReplyPacket(xml);

        free(xml);
    }

    void discoveryRequest()
    {
        Serial.println("Discovery Request Recieved.");
        networkManager.discovery();
    }

    void discoveryHeaderRequest()
    {
        Serial.println("Discovery Header Request Recieved.");
        char response[86];
        sprintf(response, "330|%s,%s", networkManager.macAddress, header);
        networkManager.sendReplyPacket(response);
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
        boolean responseRequired = statusManager.newStatusRegistration(networkManager.incomingPacket, temp);
        if (responseRequired)
        {
            networkManager.sendReplyPacket(temp);
        }
    }
};
