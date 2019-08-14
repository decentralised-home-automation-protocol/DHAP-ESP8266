#include "Status.h"
#include "Arduino.h"

class StatusManager
{
private:
    Status *deviceStatus;
    String macAddress;
    String endLeaseResponse;
    String statusResponseHeader;

    int numDevicesListening = 0;
    int currentUpdatePeriod = 0;
    unsigned long previousMillis = 0;
    unsigned long leaseLengthRemaining = 0;
    unsigned long currentMillis = 0;
    unsigned long timeSinceLastUpdate = 0;

public:
    void setStatusController(Status &devStatus, String mac)
    {
        deviceStatus = &devStatus;
        macAddress = mac;
        endLeaseResponse = "530|" + macAddress + ",T,";
        statusResponseHeader = "530|" + macAddress + ",F,";
    }

    String getStatusUpdateIfNeeded()
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
                return endLeaseResponse + deviceStatus->getStatus();
            }
            else
            {
                leaseLengthRemaining -= deltaMillis;
            }

            if (timeSinceLastUpdate >= currentUpdatePeriod)
            {
                //send the status update.
                timeSinceLastUpdate = 0;
                return statusResponseHeader + deviceStatus->getStatus();
            }
        }
        return "";
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

    String newStatusRegistration(char *registration)
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
            char reply[45];

            sprintf(reply, "510|%s,%lu,%d", macAddress.c_str(), allocatedLeasePeriod, allocatedUpdatePeriod);

            return reply;
        }
        else
        {
            return "";
        }
    }

    void removeListeningDevice()
    {
        numDevicesListening--;
        if (numDevicesListening == 0)
        {
            leaseLengthRemaining = 0;
        }
    }
};
