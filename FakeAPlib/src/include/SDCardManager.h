#ifndef SD_CARD_MANAGER_H
#define SD_CARD_MANAGER_H

#include "include.h"

class SDCardManager
{
protected:
    fs::FS _fileSystem;
    bool _sdInitialized;
    // String _logFile;

public:
    SDCardManager()
        : _fileSystem{SD_MMC}, _sdInitialized{false}
    {
    }

    SDCardManager(const SDCardManager &) = delete;

    SDCardManager operator=(const SDCardManager &) = delete;

    bool operator+(const String &path) { return writeFile(path.c_str()); }

    bool operator-(const String &path) { return deleteFile(path.c_str()); }

    inline bool isCardInit() const { return _sdInitialized; }

    inline fs::FS &getFileSystem() { return _fileSystem; }
    /*
    inline void setPath(const char* path) { _logFile = path; }

    inline void setPath(const String& path) { _logFile = path; }
    */

    static inline char *getCardType(uint8_t card)
    {
        switch (card)
        {
        case CARD_MMC:
            return "MMC";
            break;

        case CARD_SD:
            return "SD";
            break;

        case CARD_SDHC:
            return "SDHC";
            break;
        }
        return "Unknown";
    }

    static inline String getFileDir(const char *path)
    {
        String str = path;
        uint8_t index = str.lastIndexOf("/");

        return str.substring(0, index);
    }

    static inline String getFileDir(const String &path)
    {
        uint8_t index = path.lastIndexOf("/");

        return path.substring(0, index);
    }

    inline bool logEvent(const String &event)
    {
        if (!_sdInitialized)
        {
#if (WITH_ERROR_TYPE)
            Serial.println(ERROR_SD_NOT_INIT);
#endif
            return 0;
        }

        String logDir = getFileDir(LOG_FILE);
        if (!_fileSystem.exists(logDir))
        {
            if (!createDir(logDir))
                return 0;
            return writeFile(LOG_FILE, getCurrentTime() + ": " + event) ? 1 : 0;
        }
        else if (!_fileSystem.exists(LOG_FILE))
        {
            return writeFile(LOG_FILE, getCurrentTime() + ": " + event) ? 1 : 0;
        }

        File *logFile = new File(_fileSystem.open(LOG_FILE, FILE_APPEND));
        if (!*logFile)
        {
#if (WITH_ERROR_TYPE)
            Serial.printf(ERROR_FILE_OPEN, LOG_FILE);
#endif
            delete logFile;
            return 0;
        }

        logFile->println(getCurrentTime() + ": " + event);
        logFile->close();
        delete logFile;
#if (WITH_SUCCESS_MESSAGE)
        Serial.println(SUCCESS_LOG_EVENT);
#endif
        return 1;
    }

    inline bool initialize()
    {
        Serial.println("Montando la tarjeta SD....");
        SD_MMC.setPins(SD_MMC_CLK, SD_MMC_CMD, SD_MMC_D0);

        if (!SD_MMC.begin("/sdcard", true, true, SDMMC_FREQ_DEFAULT, 5))
        {
#if (WITH_ERROR_TYPE)
            Serial.println(ERROR_SD_CARD_READ);
#endif
            return 0;
        }

        uint8_t cardType = SD_MMC.cardType();
        if (cardType == CARD_NONE)
        {
#if (WITH_ERROR_TYPE)
            Serial.println(ERROR_SD_CARD_TYPE);
#endif
            return 0;
        }

#if (WITH_SUCCESS_MESSAGE)
        Serial.println(SUCCESS_SD_INIT);
#endif
        Serial.printf("Tipo de tarjeta: %s\n", getCardType(cardType));

        uint64_t cardSize = SD_MMC.cardSize() / (1024 * 1024);
        Serial.printf("Tamaño de la tarjeta SD_MMC: %lluMB\n", cardSize);

        _sdInitialized = true;

        Serial.printf("Total espacio disponible: %lluMB\r\n", SD_MMC.totalBytes() / (1024 * 1024));

        listDir("/", 0);

        return 1;
    }

    inline String readFile(const char *path)
    {
        unsigned long startTime = millis();
        if (!_sdInitialized)
        {
#if (WITH_ERROR_TYPE)
            Serial.println(ERROR_SD_NOT_INIT);
#endif
            return "\0";
        }

        File *file = new File(_fileSystem.open(path));
        if (!*file)
        {
#if (WITH_ERROR_TYPE)
            Serial.printf(ERROR_FILE_OPEN, path);
#endif
            delete file;
            return "\0";
        }

        String content = file->readStringUntil('\0');

#if (WITH_SUCCESS_MESSAGE)
        Serial.printf(SUCCESS_FILE_OPEN, file->name(), (millis() - startTime));
#endif
        file->close();
        delete file;

        return content;
    }

    inline String readFile(const String &path)
    {
        unsigned long startTime = millis();
        if (!_sdInitialized)
        {
#if (WITH_ERROR_TYPE)
            Serial.println(ERROR_SD_NOT_INIT);
#endif
            return "\0";
        }

        File *file = new File(_fileSystem.open(path));
        if (!*file)
        {
#if (WITH_ERROR_TYPE)
            Serial.printf(ERROR_FILE_OPEN, path.c_str());
#endif
            delete file;
            return "\0";
        }

        String content = file->readStringUntil('\0');

#if (WITH_SUCCESS_MESSAGE)
        Serial.printf(SUCCESS_FILE_OPEN, file->name(), (millis() - startTime));
#endif
        file->close();
        delete file;

        return content;
    }


    inline bool writeFile(const char *path, const String &content = "")
    {
        if (!_sdInitialized)
        {
#if (WITH_ERROR_TYPE)
            Serial.println(ERROR_SD_NOT_INIT);
#endif
            return 0;
        }

        File *file = new File(_fileSystem.open(path, FILE_WRITE));
        if (!*file)
        {
#if (WITH_ERROR_TYPE)
            Serial.printf(ERROR_FILE_OPEN, path);
#endif
            delete file;
            return 0;
        }
        else if (!file->println(content))
        {
#if (WITH_ERROR_TYPE)
            Serial.printf(ERROR_FILE_WRITE, path);
#endif
            delete file;
            return 0;
        }

#if (WITH_SUCCESS_MESSAGE)
        Serial.printf(SUCCESS_FILE_WRITE, file->name());
#endif
        file->close();
        delete file;
        return 1;
    }


    inline bool writeFile(const String &path, const String &content = "")
    {
        if (!_sdInitialized)
        {
#if (WITH_ERROR_TYPE)
            Serial.println(ERROR_SD_NOT_INIT);
#endif
            return 0;
        }

        File *file = new File(_fileSystem.open(path, FILE_WRITE));
        if (!*file)
        {
#if (WITH_ERROR_TYPE)
            Serial.printf(ERROR_FILE_OPEN, path.c_str());
#endif
            delete file;
            return 0;
        }
        else if (!file->println(content))
        {
#if (WITH_ERROR_TYPE)
            Serial.printf(ERROR_FILE_WRITE, path.c_str());
#endif
            delete file;
            return 0;
        }

#if (WITH_SUCCESS_MESSAGE)
        Serial.printf(SUCCESS_FILE_WRITE, file->name());
#endif
        file->close();
        delete file;
        return 1;
    }


    inline void listDir(const char *dirname, uint8_t levels)
    {
        if (!_sdInitialized)
        {
#if (WITH_ERROR_TYPE)
            Serial.println(ERROR_SD_NOT_INIT);
#endif
            return;
        }

        Serial.printf("Listing directory: %s\n", dirname);
        File *root = new File(_fileSystem.open(dirname));
        if (!*root)
        {
#if (WITH_ERROR_TYPE)
            Serial.printf(ERROR_FILE_OPEN, dirname);
#endif
            delete root;
            return;
        }
        if (!root->isDirectory())
        {
#if (WITH_ERROR_TYPE)
            Serial.printf(ERROR_FILE_TYPE, dirname);
#endif
            delete root;
            return;
        }

        File *file = new File(root->openNextFile());
        String fileList = "";
        while (*file)
        {
            if (file->isDirectory())
            {
                Serial.print("  DIR : ");
                Serial.println(file->name());
                if (!levels)
                    listDir(file->name(), levels - 1);
            }
            else
            {
                Serial.print("  FILE: ");
                Serial.print(file->name());
                Serial.print("  SIZE: ");
                Serial.println(file->size());
            }
            *file = root->openNextFile();
        }

        delete root,
               file;
    }

    inline bool createDir(const String &path)
    {
        if (!_sdInitialized)
        {
#if (WITH_ERROR_TYPE)
            Serial.println(ERROR_SD_NOT_INIT);
#endif
            return 0;
        }

        Serial.printf("Creating Dir: %s\n", path.c_str());
        if (!_fileSystem.mkdir(path))
        {
#if (WITH_ERROR_TYPE)
            Serial.printf(ERROR_DIR_CREATE, path.c_str());
#endif
            return 0;
        }

#if (WITH_SUCCESS_MESSAGE)
        Serial.printf(SUCCESS_DIR_CREATE, path.c_str());
#endif
        return 1;
    }

    inline bool createDir(const char *path)
    {
        if (!_sdInitialized)
        {
#if (WITH_ERROR_TYPE)
            Serial.println(ERROR_SD_NOT_INIT);
#endif
            return 0;
        }

        Serial.printf("Creating Dir: %s\n", path);
        if (!_fileSystem.mkdir(path))
        {
#if (WITH_ERROR_TYPE)
            Serial.printf(ERROR_DIR_CREATE, path);
#endif
            return 0;
        }

#if (WITH_SUCCESS_MESSAGE)
        Serial.printf(SUCCESS_DIR_CREATE, path);
#endif
        return 1;
    }

    inline bool removeDir(const char *path)
    {
        if (!_sdInitialized)
        {
#if (WITH_ERROR_TYPE)
            Serial.println(ERROR_SD_NOT_INIT);
#endif
            return 0;
        }

        Serial.printf("Removing Dir: %s\n", path);
        if (!_fileSystem.rmdir(path))
        {
#if (WITH_ERROR_TYPE)
            Serial.printf(ERROR_DIR_DELETE, path);
#endif
            return 0;
        }

#if (WITH_SUCCESS_MESSAGE)
        Serial.printf(SUCCESS_DIR_DELETE, path);
#endif
        return 1;
    }

    inline bool appendFile(const char *path, String &content)
    {
        if (!_sdInitialized)
        {
#if (WITH_ERROR_TYPE)
            Serial.println(ERROR_SD_NOT_INIT);
#endif
            return 0;
        }
        File *file = new File(_fileSystem.open(path, FILE_APPEND));
        if (!*file)
        {
#if (WITH_ERROR_TYPE)
            Serial.printf(ERROR_FILE_OPEN, path);
#endif
            delete file;
            return 0;
        }
        content = "\n" + content;
        if (!file->print(content))
        {
            Serial.printf(ERROR_FILE_WRITE, path);
            delete file;
            return 0;
        }

#if (WITH_SUCCESS_MESSAGE)
        Serial.printf(SUCCESS_FILE_WRITE, file->name());
#endif
        file->close();
        delete file;

        return 1;
    }

    inline bool appendFile(const String &path, String &content)
    {
        if (!_sdInitialized)
        {
#if (WITH_ERROR_TYPE)
            Serial.println(ERROR_SD_NOT_INIT);
#endif
            return 0;
        }
        File *file = new File(_fileSystem.open(path, FILE_APPEND));
        if (!*file)
        {
#if (WITH_ERROR_TYPE)
            Serial.printf(ERROR_FILE_OPEN, path.c_str());
#endif
            delete file;
            return 0;
        }
        content = "\n" + content;
        if (!file->print(content))
        {
            Serial.printf(ERROR_FILE_WRITE, path.c_str());
            delete file;
            return 0;
        }

#if (WITH_SUCCESS_MESSAGE)
        Serial.printf(SUCCESS_FILE_WRITE, file->name());
#endif
        file->close();
        delete file;

        return 1;
    }

    inline bool renameFile(const char *path1, const char *path2)
    {
        if (!_sdInitialized)
        {
#if (WITH_ERROR_TYPE)
            Serial.println(ERROR_SD_NOT_INIT);
#endif
            return 0;
        }

        if (!_fileSystem.rename(path1, path2))
        {
#if (WITH_ERROR_TYPE)
            Serial.printf(ERROR_FILE_RENAME, path1);
#endif
            return 0;
        }
        Serial.printf(SUCCESS_FILE_RENAME, path1);

        return 1;
    }

    inline bool deleteFile(const char *path)
    {
        if (!_sdInitialized)
        {
#if (WITH_ERROR_TYPE)
            Serial.println(ERROR_SD_NOT_INIT);
#endif
            return 0;
        }

        Serial.printf("Deleting file: %s\n", path);
        if (!_fileSystem.remove(path))
        {
#if (WITH_ERROR_TYPE)
            Serial.printf(ERROR_FILE_DELETE, path);
#endif
            return 0;
        }
#if (WITH_SUCCESS_MESSAGE)
        Serial.printf(SUCCESS_FILE_DELETE, path);
#endif

        return 1;
    }


    inline bool deleteFile(const String& path)
    {
        if (!_sdInitialized)
        {
#if (WITH_ERROR_TYPE)
            Serial.println(ERROR_SD_NOT_INIT);
#endif
            return 0;
        }

        Serial.printf("Deleting file: %s\n", path.c_str());
        if (!_fileSystem.remove(path))
        {
#if (WITH_ERROR_TYPE)
            Serial.printf(ERROR_FILE_DELETE, path);
#endif
            return 0;
        }
#if (WITH_SUCCESS_MESSAGE)
        Serial.printf(SUCCESS_FILE_DELETE, path);
#endif

        return 1;
    }


    inline bool deleteRecursive(const String &path)
    {
        File *file = new File(_fileSystem.open(path));
        if (!file->isDirectory())
        {
            file->close();
            _fileSystem.remove(path);
            return 1;
        }

        file->rewindDirectory();
        while (true)
        {
            File entry = file->openNextFile();
            if (!entry)
                break;

            String entryPath = path + "/" + entry.name();
            if (entry.isDirectory())
            {
                entry.close();
                deleteRecursive(entryPath);
            }
            else
            {
                entry.close();
                _fileSystem.remove(entryPath);
            }
            yield();
        }

        _fileSystem.rmdir(path);
        file->close();
        delete file;

        return 1;
    }
};

#endif // SD_CARD_MANAGER_H