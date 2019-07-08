#include "NetworkManager.h"

class IoTDevice
{
private:
    NetworkManager networkManager;
    FileManager fileManager;

    char *DEFAULT_SSID = "TP-LINK_AE045A";
    char *DEFAULT_PASSWORD = "0358721743";

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
            networkManager.joinNetwork(DEFAULT_SSID, DEFAULT_PASSWORD);
        }
    }

    bool commandRecieved(char *iotCommand)
    {
        bool newIncomingCommandReceived = networkManager.commandRecieved();
        if (newIncomingCommandReceived)
        {
            return handleIncomingPacket(iotCommand);
        }
        else
        {
            return false;
        }
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
            joinNewNetwork();
        }
        return false;
    }

    bool isCommandUiRequest()
    {
        return networkManager.incomingPacket[0] == 'U' && networkManager.incomingPacket[1] == 'I';
    }

    void joinNewNetwork()
    {
        char delim[] = ":";
        char *SSID = strtok(networkManager.incomingPacket, delim);
        char *password = strtok(NULL, delim);
        networkManager.joinNetwork(SSID, password);
    }
};
