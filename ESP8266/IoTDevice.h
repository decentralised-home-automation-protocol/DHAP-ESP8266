#include "NetworkManager.h"

class IoTDevice
{
private:
    NetworkManager networkManager;
    FileManager fileManager;

    char *ssid;
    char *password;

public:
    void setup(bool setupAP)
    {
        fileManager.mountFileSystem();

        if (setupAP)
        {
            networkManager.setupAccessPoint();
        }
        else
        {
            // fileManager.saveNetworkCredentials("TP-LINK_AE045A", "0358721743");
            String credentials = fileManager.getNetworkCredentials();

            if (credentials.length() == 0)
            {
                Serial.println("No credentials found!");
                networkManager.setupAccessPoint();
            }
            else
            {
                char *creds = new char[credentials.length()];
                strcpy(creds, credentials.c_str());
                tolkenizeCredentials(creds);
                networkManager.joinNetwork(ssid, password);
            }
        }
    }

    bool commandRecieved(char *iotCommand)
    {
        bool newIncomingCommandReceived = networkManager.commandRecieved();
        if (newIncomingCommandReceived)
        {
            return handleIncomingPacket(iotCommand);
        }
        return false;
    }

    bool handleIncomingPacket(char *iotCommand)
    {
        if (networkManager.hasJoinedNetwork)
        {
            if (isCommandUiRequest())
            {
                networkManager.sendXMLfile();
            }
            else
            {
                networkManager.getRecentPacket(iotCommand);
                return true;
            }
        }
        else
        {
            tolkenizeCredentials(networkManager.incomingPacket);
            fileManager.saveNetworkCredentials(ssid, password);
            networkManager.joinNetwork(ssid, password);
        }
        return false;
    }

    bool isCommandUiRequest()
    {
        return networkManager.incomingPacket[0] == 'U' && networkManager.incomingPacket[1] == 'I';
    }

    void tolkenizeCredentials(char *credentials)
    {
        ssid = strtok(credentials, ":");
        password = strtok(NULL, ":");
    }
};
