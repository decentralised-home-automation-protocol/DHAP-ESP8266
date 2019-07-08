#include "NetworkManager.h"

class IoTDevice
{
public:
    NetworkManager net;
    FileManager fileManager;

    char *DEFAULT_SSID = "TP-LINK_AE045A";
    char *DEFAULT_PASSWORD = "0358721743";

    void setup(bool setupAP)
    {
        fileManager.mountFileSystem();

        if (setupAP)
        {
            net.setupAccessPoint();
        }
        else
        {
            net.joinNetwork(DEFAULT_SSID, DEFAULT_PASSWORD);
        }
    }

    bool commandRecieved(char *iotCommand)
    {
        bool newIncomingCommandReceived = net.commandRecieved();
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
        if (net.hasJoinedNetwork)
        {
            if (isCommandUiRequest())
            {
                net.sendXMLfile();
            }
            else
            {
                net.getRecentPacket(iotCommand);
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
        return net.incomingPacket[0] == 'U' && net.incomingPacket[1] == 'I';
    }

    void joinNewNetwork()
    {
        char *SSID;
        char *password;
        char delim[] = ":";
        SSID = strtok(net.incomingPacket, delim);
        password = strtok(NULL, delim);
        net.joinNetwork(SSID, password);
    }
};
