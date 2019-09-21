#include "FS.h"

class FileManager
{
private:
    char *xmlFileName = "/layout.xml";
    char *headerFileName = "/header.txt";
    char *networkCredentialsFileName = "/credentials.txt";

public:
    int layoutFileSize;

    void mountFileSystem()
    {
        if (SPIFFS.begin())
        {
            Serial.println("mount success");
            File file = SPIFFS.open(xmlFileName, "r");
            layoutFileSize = file.size();
            file.close();
        }
        else
        {
            Serial.println("mount failed");
        }
    }

    void getFileHeader(char *header)
    {
        File file = SPIFFS.open(headerFileName, "r");
        Serial.println("Getting file header");
        file.readBytes(header, file.size());
        file.close();

        if (strlen(header) < 1){
            Serial.println("No header file found! Creating a header file...");
            sprintf(header, "0,name not set,location not set");

            File file2 = SPIFFS.open(headerFileName, "w");
            file2.write(header, 64);
            file2.close();
        }
        Serial.printf("File header: %s\n", header);
    }

    void setFileHeader(char currentHeaderVersion, char *name, char *location)
    {
        Serial.println("Setting file header");

        int currentVersionInt = currentHeaderVersion - 48;
        currentVersionInt++;
        if (currentVersionInt > 9)
        {
            currentVersionInt = 0;
        }

        //Write new header to file.
        char header[64];
        sprintf(header, "%d,%s,%s", currentVersionInt, name, location);

        File file2 = SPIFFS.open(headerFileName, "w");
        file2.write(header, 64);
        file2.close();
    }

    void readFile(char *response)
    {
        File file = SPIFFS.open(xmlFileName, "r");

        file.readBytes(response, layoutFileSize);

        file.close();
    }

    void getSavedNetworkCredentials(char *creds)
    {
        Serial.println("Getting network credentials from file");
        File file = SPIFFS.open(networkCredentialsFileName, "r");

        file.readBytes(creds, layoutFileSize);

        file.close();
    }

    void saveNetworkCredentials(char *SSID, char *password)
    {
        Serial.println("Saving network credentials to file");

        char credentials[64];
        sprintf(credentials, "%s,%s", SSID, password);

        File file = SPIFFS.open(networkCredentialsFileName, "w");
        file.write(credentials, 64);
        file.close();
    }
};
