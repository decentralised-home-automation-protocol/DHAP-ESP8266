#include "NetworkManager.h"
#include "FileManager.h"
#include "Status.h"

class IoTDevice
{
private:
    NetworkManager networkManager;
    FileManager fileManager;
    Status *deviceStatus;

    char *ssid;
    char *password;

    int numDevicesListening = 0;
    int minUpdatePeriod = 0;
    unsigned long previousMillis = 0;
    unsigned long leaseLengthRemaining = 0;
    unsigned long currentMillis = 0;
    unsigned long timeSinceLastUpdate = 0;

public:
    void setup(bool setupAP, Status &devStatus)
    {
        deviceStatus = &devStatus;

        newStatusRegistration("ST,20000,5000,T");

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
        sendStatusUpdateIfNeeded();

        if (networkManager.newCommandRecieved())
        {
            return handleIncomingPacket(iotCommand);
        }
        return false;
    }

    void sendStatusUpdateIfNeeded()
    {
        //check if someone is listening with a valid lease.
        if (numDevicesListening > 0)
        {
            //check if its time to send a status update
            previousMillis = currentMillis;
            currentMillis = millis();

            unsigned long deltaMillis = currentMillis - previousMillis;

            timeSinceLastUpdate += deltaMillis;
            if (timeSinceLastUpdate >= minUpdatePeriod)
            {
                //send the status update.
                timeSinceLastUpdate = 0;
                networkManager.broadcastStatus(deviceStatus->getStatus());
            }

            if (deltaMillis >= leaseLengthRemaining)
            {
                //lease has expired.
                numDevicesListening = 0;
                leaseLengthRemaining = 0;
                networkManager.broadcastStatus("Lease Expired.");
            }
            else
            {
                leaseLengthRemaining -= deltaMillis;
            }
        }
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
                newStatusRegistration(networkManager.incomingPacket);
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

    void newStatusRegistration(char *registration)
    {
        char *request = strtok(registration, ",");
        int leasePeriod = atoi(strtok(NULL, ","));
        int updatePeriod = atoi(strtok(NULL, ","));
        char *replyRequired = strtok(NULL, ",");

        addNewListeningDevice(leasePeriod, updatePeriod);
    }

    void addNewListeningDevice(unsigned long leasePeriod, int updatePeriod)
    {
        if (numDevicesListening == 0)
        {
            leaseLengthRemaining = leasePeriod;
            minUpdatePeriod = updatePeriod;
            timeSinceLastUpdate = 0;
            previousMillis = millis();
        }
        else
        {
            if (leasePeriod > leaseLengthRemaining)
            {
                leaseLengthRemaining = leasePeriod;
            }

            if (minUpdatePeriod > updatePeriod)
            {
                minUpdatePeriod = updatePeriod;
            }
        }
        numDevicesListening++;
    }
};