#include "NetworkManager.h"
#include "FileManager.h"
#include "StatusManager.h"

class IoTDevice
{
private:
    NetworkManager networkManager;
    FileManager fileManager;
    StatusManager statusManager;
    Status *deviceStatus;

    const int PACKET_TYPE_HEADER_LENGTH = 4;

    char *ssid;
    char *password;
    char temp[255];
    char name[32];
    char location[32];
    char headerVersion;
public:
    void setup(Status &userDeviceStatus, bool forceSetupAP)
    {
        deviceStatus = &userDeviceStatus;
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

        getHeader();

        statusManager.setStatusController(userDeviceStatus, networkManager.macAddress);
    }

    void getHeader(){
        char header[65];
        fileManager.getFileHeader(header);

        headerVersion = header[0];

        strtok(header, ",");
        strcpy(name, strtok(NULL, ","));
        strcpy(location, strtok(NULL, ","));

        networkManager.headerVersion = headerVersion;
    }

    void loop()
    {
        boolean newStatus = statusManager.getStatusUpdateIfNeeded(temp);
        if (newStatus)
        {
            networkManager.broadcastStatus(temp);
        }

        if (networkManager.newCommandReceived())
        {
            handleIncomingPacket();
        }

        if (!networkManager.hasJoinedNetwork)
        {
            networkManager.joinWiFiLoop(ssid, password);
        }
    }

    void handleIncomingPacket()
    {
        switch (networkManager.getRecentPacketType())
        {
        case 100: //Joining credentials
            joiningPacket();
            return;
        case 200: //UI Request
            uiRequest();
            return;
        case 300: //Discovery Request
            discoveryRequest();
            return;
        case 320: //Discovery Header Request
            discoveryHeaderRequest();
            return;
        case 400: //IoT Command
            getIoTCommand();

            //Device state has changed. Send a status update.
            statusManager.getStatusUpdate(temp);
            networkManager.broadcastStatus(temp);
            return;
        case 500: //Status Lease Request
            statusLeaseRequest();
            return;
        case 520: //Leave Status Lease
            statusManager.removeListeningDevice();
            return;
        case 600: //Change header name
            changeHeaderName();
            getHeader();
            return;
        case 610: //Change header location
            changeHeaderLocation();
            getHeader();
            return;
        default:
            return;
        };
        return;
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
        Serial.println("UI Request Received.");
        int size = fileManager.layoutFileSize + PACKET_TYPE_HEADER_LENGTH;
        char *xml = (char *)malloc(size);
        sprintf(xml, "210|");

        fileManager.readFile(xml + PACKET_TYPE_HEADER_LENGTH);

        //Ensures there isn't any junk after the xml.
        xml[size] = '\0';

        networkManager.sendReplyPacket(xml);

        free(xml);
    }

    void discoveryRequest()
    {
        Serial.println("Discovery Request Received.");
        networkManager.discovery();
    }

    void discoveryHeaderRequest()
    {
        Serial.println("Discovery Header Request Received.");
        char response[86];
        sprintf(response, "330|%s,%c,%s,%s", networkManager.macAddress, headerVersion, name, location);
        networkManager.sendReplyPacket(response);
    }

    void getIoTCommand()
    {
        char iotCommand[255];
        networkManager.getRecentPacket(temp);
        strcpy(iotCommand, temp + PACKET_TYPE_HEADER_LENGTH);

        deviceStatus->executeCommand(getCommandId(iotCommand), getCommandData(iotCommand));
    }

    void statusLeaseRequest()
    {
        Serial.println("Status Request Received. Adding to List...");
        boolean responseRequired = statusManager.newStatusRegistration(networkManager.incomingPacket, temp);
        if (responseRequired)
        {
            networkManager.sendReplyPacket(temp);
        }
    }

    void changeHeaderName(){
        networkManager.getRecentPacket(temp);
        strtok(temp, "|"); //type
        fileManager.setFileHeader(headerVersion, strtok(NULL, "|"), location);
    }

    void changeHeaderLocation()
    {
        networkManager.getRecentPacket(temp);
        strtok(temp, "|"); //type
        fileManager.setFileHeader(headerVersion, name, strtok(NULL, "|"));
    }
    
    String getCommandId(char *command)
    {
        char commandData[255];
        strcpy(commandData, command);
        return strtok(commandData, "=");
    }

    String getCommandData(char *command)
    {
        char commandData[255];
        strcpy(commandData, command);
        strtok(commandData, "=");
        return strtok(NULL, "=");
    }
};
