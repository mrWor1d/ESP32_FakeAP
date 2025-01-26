#ifndef SD_CARD_MANAGER_CPP
#define SD_CARD_MANAGER_CPP

#include "SDCardManager.h"


SDCardManager::SDCardManager()
    : _fileSystem{SD_MMC}, _sdInitialized{false}
{
}

bool SDCardManager::operator+(const String &path) { return writeFile(path.c_str()); }

bool SDCardManager::operator-(const String &path) { return deleteFile(path.c_str()); }

bool SDCardManager::isCardInit() const { return _sdInitialized; }

fs::SDMMCFS& SDCardManager::getFileSystem() { return _fileSystem; }


char* SDCardManager::getCardType(uint8_t card)
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


String SDCardManager::getFileDir(const char *path)
{
    String str = path;
    uint8_t index = str.lastIndexOf("/");

    return str.substring(0, index);
}


String SDCardManager::getFileDir(const String &path)
{
    uint8_t index = path.lastIndexOf("/");

    return path.substring(0, index);
}


bool SDCardManager::logEvent(const String &event)
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
         //crear la carpeta y el archivo de los logs en caso de que no exista
        if (!createDir(logDir))
            return 0;
        return writeFile(LOG_FILE, getCurrentTime() + "-> " + event) ? 1 : 0;
    }
    else if (!_fileSystem.exists(LOG_FILE))
    {
        //crear solo el archivo en caso de que no exista
        return writeFile(LOG_FILE, getCurrentTime() + "-> " + event) ? 1 : 0;
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

    //añadir el log en una nueva linea del archivo existente
    logFile->println(getCurrentTime() + "-> " + event);
    logFile->close();
    delete logFile;
#if (WITH_SUCCESS_MESSAGE)
    Serial.println(SUCCESS_LOG_EVENT);
#endif
    return 1;
}


bool SDCardManager::initialize()
{
    Serial.println("Montando la tarjeta SD....");

    /**
     * configurar los pines para una placa con lector integrado
     * 
     * ! Solo cambiar los pines si se usa un tipo de tarjeta diferente
     * ! Que no tiene lector integrado 
     */
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

    Serial.printf("Tamaño de la tarjeta SD_MMC: %s\n", getReadableSize(SD_MMC.cardSize()));

    Serial.printf("Total espacio disponible: %s\r\n", getReadableSize(SD_MMC.totalBytes()));

    //listDir("/", 0);

    _sdInitialized = true;

    return 1;
}


String SDCardManager::readFile(const char *path)
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

String SDCardManager::readFile(const String &path)
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

    //leer el archivo hasta el final
    String content = file->readStringUntil('\0');

#if (WITH_SUCCESS_MESSAGE)
    Serial.printf(SUCCESS_FILE_OPEN, file->name(), (millis() - startTime));
#endif
    file->close();
    delete file;

    return content;
}


bool SDCardManager::writeFile(const char *path, const String &content)
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


bool SDCardManager::writeFile(const String &path, const String &content)
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

/*
void SDCardManager::listDir(const char *dirname, uint8_t levels)
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
*/


bool SDCardManager::createDir(const String &path)
{
    if (!_sdInitialized)
    {
#if (WITH_ERROR_TYPE)
        Serial.println(ERROR_SD_NOT_INIT);
#endif
        return 0;
    }

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


bool SDCardManager::createDir(const char *path)
{
    if (!_sdInitialized)
    {
#if (WITH_ERROR_TYPE)
        Serial.println(ERROR_SD_NOT_INIT);
#endif
        return 0;
    }

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


bool SDCardManager::removeDir(const char *path)
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


bool SDCardManager::appendFile(const char *path, String &content)
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


bool SDCardManager::appendFile(const String &path, String &content)
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


bool SDCardManager::renameFile(const char *path1, const char *path2)
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

bool SDCardManager::deleteFile(const char *path)
{
    if (!_sdInitialized)
    {
#if (WITH_ERROR_TYPE)
        Serial.println(ERROR_SD_NOT_INIT);
#endif
        return 0;
    }

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


bool SDCardManager::deleteFile(const String& path)
{
    if (!_sdInitialized)
    {
#if (WITH_ERROR_TYPE)
        Serial.println(ERROR_SD_NOT_INIT);
#endif
        return 0;
    }

    if (!_fileSystem.remove(path))
    {
#if (WITH_ERROR_TYPE)
        Serial.printf(ERROR_FILE_DELETE, path);
#endif
        return 0;
    }
#if (WITH_SUCCESS_MESSAGE)
    Serial.printf(SUCCESS_FILE_DELETE, path.c_str());
#endif

    return 1;
}


bool SDCardManager::deleteRecursive(const String &path)
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

#endif // SD_CARD_MANAGER_CPP