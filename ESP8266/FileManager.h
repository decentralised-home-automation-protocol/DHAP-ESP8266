#include "FS.h"

class FileManager
{
public:
    char *xmlFileName = "/TV.xml";

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
};