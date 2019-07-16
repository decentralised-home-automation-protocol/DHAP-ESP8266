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

public:
    void setup(bool setupAP, Status &deviceStatus)
    {
        statusManager.setStatusController(deviceStatus);

        fileManager.mountFileSystem();

        if (setupAP)
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
                tolkenizeCredentials(creds); //Gets SSID and password from saved credentials string.

                if (!networkManager.joinNetwork(ssid, password))
                {
                    Serial.println("Failed to join network!");
                    networkManager.setupAccessPoint();
                }
            }
        }
    }

    bool commandRecieved(char *iotCommand)
    {
        String status = statusManager.sendStatusUpdateIfNeeded();
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
        if (networkManager.hasJoinedNetwork)
        {
            if (isUIRequest())
            {
                Serial.println("UI Request Recieved.");
                String response = fileManager.readFile();
                networkManager.sendReplyPacket(response);
                Serial.println("XML File sent.");
            }
            else if (isDiscoveryRequest())
            {
                Serial.println("Discovery Request Recieved.");
                String response = networkManager.getLocalIP();
                networkManager.sendReplyPacket(response);
                Serial.println("Discovery Packet Sent.");
            }
            else if (isStatusRequest())
            {
                Serial.println("Status Request Recieved. Adding to List...");
                statusManager.newStatusRegistration(networkManager.incomingPacket);
            }
            else
            {
                Serial.println("IoT Command Recieved.");
                networkManager.getRecentPacket(iotCommand);
                return true;
            }
        }
        else
        {
            attemptToJoinNetwork();
        }
        return false;
    }

    void attemptToJoinNetwork()
    {
        tolkenizeCredentials(networkManager.incomingPacket);
        if (networkManager.joinNetwork(ssid, password))
        {
            fileManager.saveNetworkCredentials(ssid, password);
        }
        else
        {
            Serial.println("Failed to join network!");
        }
    }

    bool isUIRequest()
    {
        return networkManager.incomingPacket[0] == 'U' && networkManager.incomingPacket[1] == 'I';
    }

    bool isDiscoveryRequest()
    {
        return networkManager.incomingPacket[0] == 'D' && networkManager.incomingPacket[1] == 'I';
    }

    bool isStatusRequest()
    {
        return networkManager.incomingPacket[0] == 'S' && networkManager.incomingPacket[1] == 'T';
    }

    void tolkenizeCredentials(char *credentials)
    {
        ssid = strtok(credentials, ",");
        password = strtok(NULL, ",");
    }
};