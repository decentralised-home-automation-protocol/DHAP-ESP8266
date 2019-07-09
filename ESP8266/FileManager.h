#include "FS.h"

class FileManager
{
private:
    char *xmlFileName = "/TV.xml";
    char *networkCredentialsFileName = "/credentials.txt";

public:
    void mountFileSystem()
    {
        if (SPIFFS.begin())
        {
            Serial.println("mount success");
        }
        else
        {
            Serial.println("mount failed");
        }
    }

    String readFile()
    {
        File file = SPIFFS.open(xmlFileName, "r");
        String response;

        while (file.position() < file.size())
        {
            response += file.readString();
        }
        file.close();

        return response;
    }

    String getNetworkCredentials()
    {
        Serial.println("Getting network credentials from file");
        File file = SPIFFS.open(networkCredentialsFileName, "r");
        String credentials;

        while (file.position() < file.size())
        {
            credentials += file.readString();
        }
        file.close();
        return credentials;
    }

    void saveNetworkCredentials(String SSID, String password)
    {
        Serial.println("Saving network credentials to file");

        File file = SPIFFS.open(networkCredentialsFileName, "w");
        String credentials = SSID + ":" + password;
        file.write(credentials.c_str());
        file.close();
    }
};