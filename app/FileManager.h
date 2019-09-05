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

        char *xml = (char *)malloc(layoutFileSize);

        file.readBytes(xml, layoutFileSize);

        findString(xml, nameStart, nameEnd, name);

        findString(xml, roomStart, roomEnd, room);

        file.close();

        sprintf(header, "%s,%s", name, room);
        free(xml);
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

    void readFile(char *response)
    {
        File file = SPIFFS.open(xmlFileName, "r");

        file.readBytes(response, layoutFileSize);

        file.close();
    }

    void getSavedNetworkCredentials(char * creds)
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
