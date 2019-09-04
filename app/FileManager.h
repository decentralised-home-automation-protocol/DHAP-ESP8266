#include "FS.h"

class FileManager
{
private:
    char *xmlFileName = "/layout.xml";
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

    void getFileHeader(char* header)
    {
        File file = SPIFFS.open(xmlFileName, "r");
        char name[30];
        char room[30];

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
    }

    void findString(const char *line, char *start, char *end, char *dest)
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

    void readFile(String *response)
    {
        File file = SPIFFS.open(xmlFileName, "r");

        while (file.position() < file.size())
        {
            *response += file.readString();
        }

        file.close();
    }

    void getSavedNetworkCredentials(char * creds)
    {
        Serial.println("Getting network credentials from file");
        File file = SPIFFS.open(networkCredentialsFileName, "r");
        sprintf(creds, "");

        while (file.position() < file.size())
        {
            strcat(creds, file.readString().c_str());
        }
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
