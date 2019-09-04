#include "Status.h"
#include "Arduino.h"

class StatusManager
{
private:
    Status *deviceStatus;
    char endLeaseResponse[25];
    char statusResponseHeader[25];
    char responseRequiredHeader[25];

    int numDevicesListening = 0;
    int currentUpdatePeriod = 0;
    unsigned long previousMillis = 0;
    unsigned long leaseLengthRemaining = 0;
    unsigned long currentMillis = 0;
    unsigned long timeSinceLastUpdate = 0;

public:
    void setStatusController(Status &devStatus, const char *mac)
    {
        deviceStatus = &devStatus;
        sprintf(endLeaseResponse, "530|%s,T,", mac);
        sprintf(statusResponseHeader, "530|%s,F,", mac);
        sprintf(responseRequiredHeader, "510|%s,", mac);
    }

    boolean getStatusUpdateIfNeeded(char *status)
    {
        //check if someone is listening with a valid lease.
        if (numDevicesListening > 0)
        {
            //check if its time to send a status update
            currentMillis = millis();
            unsigned long deltaMillis = currentMillis - previousMillis;
            previousMillis = currentMillis;
            timeSinceLastUpdate += deltaMillis;

            if (deltaMillis >= leaseLengthRemaining)
            {
                //lease has expired.
                numDevicesListening = 0;
                leaseLengthRemaining = 0;
                sprintf(status, "%s%s", endLeaseResponse, deviceStatus->getStatus().c_str());
                return true;
            }
            else
            {
                leaseLengthRemaining -= deltaMillis;
            }

            if (timeSinceLastUpdate >= currentUpdatePeriod)
            {
                //send the status update.
                timeSinceLastUpdate = 0;
                sprintf(status, "%s%s", statusResponseHeader, deviceStatus->getStatus().c_str());
                return true;
            }
        }
        return false;
    }

    void addNewListeningDevice(unsigned long leasePeriod, int updatePeriod)
    {
        if (numDevicesListening == 0)
        {
            leaseLengthRemaining = leasePeriod;
            currentUpdatePeriod = updatePeriod;
            timeSinceLastUpdate = 0;
            previousMillis = millis();
        }
        else
        {
            if (leasePeriod > leaseLengthRemaining)
            {
                leaseLengthRemaining = leasePeriod;
            }

            if (currentUpdatePeriod > updatePeriod)
            {
                currentUpdatePeriod = updatePeriod;
            }
        }
        numDevicesListening++;
    }

    boolean newStatusRegistration(char *registration, char *response)
    {
        char *request = strtok(registration, "|,");
        int leasePeriod = atoi(strtok(NULL, "|,"));
        int updatePeriod = atoi(strtok(NULL, "|,"));
        char *replyRequired = strtok(NULL, "|,");

        //ensure values are within max and min lengths.
        int deviceMaxLeaseLength = deviceStatus->getMaxLeaseLength();
        int deviceMinUpdatePeriod = deviceStatus->getMinUpdatePeriod();
        unsigned long allocatedLeasePeriod = deviceMaxLeaseLength > leasePeriod ? leasePeriod : deviceMaxLeaseLength;
        int allocatedUpdatePeriod = deviceMinUpdatePeriod > updatePeriod ? deviceMinUpdatePeriod : updatePeriod;

        addNewListeningDevice(allocatedLeasePeriod, allocatedUpdatePeriod);

        if (replyRequired[0] == 'T')
        {
            sprintf(response, "%s%lu,%d", responseRequiredHeader, allocatedLeasePeriod, allocatedUpdatePeriod);
            return true;
        }
        return false;
    }

    void removeListeningDevice()
    {
        numDevicesListening--;
        if (numDevicesListening == 0 || numDevicesListening < 0)
        {
            leaseLengthRemaining = 0;
            numDevicesListening = 0;
        }
    }
};
