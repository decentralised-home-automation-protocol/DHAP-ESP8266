#include "NetworkManager.h"
#include "FileManager.h"
#include "StatusManager.h"

class IoTDevice
{
private:
    NetworkManager networkManager;
    FileManager fileManager;
    StatusManager statusManager;
    String header;

    char *ssid;
    char *password;
    char temp[255];

    char *discoveryHeader;

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
            String credentials = fileManager.getSavedNetworkCredentials();

            if (credentials.length() == 0)
            {
                Serial.println("No credentials found!");
                networkManager.setupAccessPoint();
            }
            else
            {
                char *creds = new char[credentials.length()];
                strcpy(creds, credentials.c_str());
                ssid = strtok(creds, "|,");
                password = strtok(NULL, "|,");

                if (!networkManager.joinNetwork(ssid, password, false))
                {
                    Serial.println("Failed to join network!");
                    networkManager.setupAccessPoint();
                }
            }
        }
        header = fileManager.getFileHeader();
        Serial.println("File header: " + header);

        statusManager.setStatusController(deviceStatus, networkManager.getMacAddress());
    }

    bool commandRecieved(char *iotCommand)
    {
        char status[150];
        boolean newStatus = statusManager.getStatusUpdateIfNeeded(status);
        if (newStatus)
        {
            networkManager.broadcastStatus(status);
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
            Serial.println("Unknown Packet");
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
        String response = "210|";
        fileManager.readFile(&response);
        networkManager.sendReplyPacket(response.c_str());
        Serial.println("XML File sent.");
    }

//    void uiRequest()
//    {
//        Serial.println("UI Request Recieved.");
//        char file[4087];
//        strcat(file, "210|");
//        fileManager.readFile(file);
//        file[sizeof(file)] = '\0';
//
//        String f = file;
//
//        networkManager.sendReplyPacket(f);
//        Serial.println("XML File sent.");
//    }

    void discoveryRequest()
    {
        Serial.println("Discovery Request Recieved.");
        networkManager.discovery();
    }

    void discoveryHeaderRequest()
    {
        Serial.println("Discovery Header Request Recieved.");  
        char response[22+header.length()];
        sprintf(response, "330|%s,%s", networkManager.getMacAddress(), header.c_str());
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
        String response = statusManager.newStatusRegistration(networkManager.incomingPacket);
        if (response.length() > 0)
        {
            networkManager.sendReplyPacket(response.c_str());
        }
    }
};
