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

    String getFileHeader()
    {
        File file = SPIFFS.open(xmlFileName, "r");
        char name[30];
        char room[30];
        char header[60];

        Serial.println("Getting file header");

        char *nameStart = "<name>";
        char *nameEnd = "</name>";
        char *roomStart = "<room>";
        char *roomEnd = "</room>";

        while (file.position() < file.size())
        {
            String lineString = file.readString();
            const char *line = lineString.c_str();

            findString(line, nameStart, nameEnd, name);

            findString(line, roomStart, roomEnd, room);
        }

        file.close();

        sprintf(header, "%s,%s", name, room);

        return header;
    }

    void findString(const char *line, char *start, char *end, char* dest)
    {
        char *result = strstr(line, start);
        if (result != NULL)
        {
            int startPosition = result - line;

            result = strstr(line, end);
            int endPosition = result - line;

            int length = endPosition - startPosition - strlen(start);
            int offset = startPosition + strlen(start);

            strncpy(dest, line + offset, length);

            dest[length] = '\0';
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

    String getSavedNetworkCredentials()
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
        String credentials = SSID + "," + password;
        file.write(credentials.c_str());
        file.close();
    }
};