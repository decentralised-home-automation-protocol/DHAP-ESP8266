#include "Status.h"
#include "Arduino.h"

class StatusManager
{
private:
    Status *deviceStatus;

    int numDevicesListening = 0;
    int minUpdatePeriod = 0;
    unsigned long previousMillis = 0;
    unsigned long leaseLengthRemaining = 0;
    unsigned long currentMillis = 0;
    unsigned long timeSinceLastUpdate = 0;

public:
    void setStatusController(Status &devStatus)
    {
        deviceStatus = &devStatus;
    }

    String sendStatusUpdateIfNeeded()
    {
        //check if someone is listening with a valid lease.
        if (numDevicesListening > 0)
        {
            //check if its time to send a status update
            currentMillis = millis();
            unsigned long deltaMillis = currentMillis - previousMillis;
            previousMillis = currentMillis;

            timeSinceLastUpdate += deltaMillis;
            if (timeSinceLastUpdate >= minUpdatePeriod)
            {
                //send the status update.
                timeSinceLastUpdate = 0;
                return deviceStatus->getStatus();
                // networkManager.broadcastStatus(deviceStatus->getStatus());
            }

            if (deltaMillis >= leaseLengthRemaining)
            {
                //lease has expired.
                numDevicesListening = 0;
                leaseLengthRemaining = 0;
                return "Lease Expired.";
                // networkManager.broadcastStatus("Lease Expired.");
            }
            else
            {
                leaseLengthRemaining -= deltaMillis;
            }
        }
        return "";
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

    void newStatusRegistration(char *registration)
    {
        char *request = strtok(registration, ",");
        int leasePeriod = atoi(strtok(NULL, ","));
        int updatePeriod = atoi(strtok(NULL, ","));
        char *replyRequired = strtok(NULL, ",");

        addNewListeningDevice(leasePeriod, updatePeriod);
    }
};