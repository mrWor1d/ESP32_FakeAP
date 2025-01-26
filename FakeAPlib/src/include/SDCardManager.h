/**
 * @file WifiCaptivePortal.h
 * 
 * @author NGUEYOU SIMO, Neil L.; MORENO ROMO, Lucas; RUBIO JIMÉNEZ, Mario
 * 
 * @brief Este archivo declara la clase SDCardManager, que se encarga de la gestión de archivos
 *        y del sistema de archivos.
 */


#ifndef SD_CARD_MANAGER_H
#define SD_CARD_MANAGER_H

#include "include.h"


/**
 * @brief Clase que gestiona las operaciones con la tarjeta SD.
 *        Actua como un wrapper para la clase fs::SDMMCFS y sus funciones
 * 
 * Esta clase proporciona una interfaz para realizar operaciones básicas de
 * sistema de archivos en una tarjeta SD, incluyendo lectura, escritura,
 * creación y eliminación de archivos y directorios.
 * 
 * @details La clase maneja:
 * - Inicialización de la tarjeta SD
 * - Operaciones de lectura/escritura de archivos
 * - Gestión de directorios
 * - Registro de eventos (logging)
 * - Operaciones recursivas en el sistema de archivos
 * 
 * La clase utiliza el sistema de archivos SDMMCFS para todas sus operaciones
 * y mantiene un estado interno que indica si la tarjeta está inicializada.
 * 
 * @note La clase implementa el patrón Singleton para evitar múltiples instancias
 * que acceden simultáneamente a la tarjeta SD.
 * 
 * 
 * @see fs::SDMMCFS
 */
class SDCardManager
{
protected:
    fs::SDMMCFS& _fileSystem;    /**< Variable que representa el sistema de archivo usado */
    bool         _sdInitialized; /**< Variable para comprobar que la tarjeta está inicializada antes de hacer cualquiera cosa */
    // String _logFile;

public:

    /**
     * @brief Constructor por defecto que inicializa los miembros de la clase.
     *        Asigna el sistema de archivos por defecto y marca la SD como no inicializada.
     */
    SDCardManager();

    SDCardManager(const SDCardManager &) = delete;

    SDCardManager operator=(const SDCardManager &) = delete;

    /**
     * @brief Operador definido para escribir un archivo.
     * @param[in] path Ruta del archivo donde se escribirá.
     * @return true si la operación tuvo éxito, false en caso contrario.
     */
    bool operator+(const String &path);

    /**
     * @brief Operador definido para eliminar un archivo.
     * @param[in] path Ruta del archivo a eliminar.
     * @return true si la operación tuvo éxito, false en caso contrario.
     */
    bool operator-(const String &path);

    /**
     * @brief Indica si la tarjeta SD ha sido inicializada.
     * @return true si está inicializada, false en caso contrario.
     */
    bool isCardInit() const;

    /**
     * @brief Devuelve la referencia al sistema de archivos asociado a la tarjeta SD.
     * @return Referencia al objeto utilizado para la operación con archivos.
     */
    fs::SDMMCFS& getFileSystem();

    /**
     * @brief Retorna una cadena con el tipo de tarjeta SD detectada.
     * @param[in] card Identificador del tipo de tarjeta devuelto por el hardware.
     * @return Cadena con la descripción del tipo de tarjeta.
     */
    static char *getCardType(uint8_t card);

    /**
     * @brief Obtiene el directorio dado el camino absoluto de un archivo.
     * @param[in] path Ruta completa de un archivo, como cadena de caracteres.
     * @return Cadena que representa el directorio del archivo.
     */
    static String getFileDir(const char *path);

    /**
     * @brief Sobrecarga para obtener el directorio de un archivo a partir de un String.
     * @param[in] path Ruta completa de un archivo, como objeto String.
     * @return Cadena que representa el directorio del archivo.
     */
    static String getFileDir(const String &path);

    /**
     * @brief Registra un evento en el archivo de log designado.
     * @param[in] event Mensaje o información a ser registrada en el archivo de log.
     * @return true si se logró registrar el evento, false de lo contrario.
     */
    bool logEvent(const String &event);

    /**
     * @brief Inicializa la tarjeta SD y muestra información sobre ella.
     * @return true si la tarjeta se inicializó correctamente, false en caso de error.
     */
    bool initialize();

    /**
     * @brief Lee el contenido de un archivo en la tarjeta SD.
     * @param[in] path Ruta del archivo a leer.
     * @return Contenido del archivo en formato String, o cadena vacía si ocurre un error.
     */
    String readFile(const char *path);

    /**
     * @brief Sobrecarga para leer el contenido de un archivo a partir de un String.
     * @param[in] path Ruta del archivo a leer.
     * @return Contenido del archivo en formato String, o cadena vacía si ocurre un error.
     */
    String readFile(const String &path);

    /**
     * @brief Escribe contenido en un archivo (creándolo si no existe).
     * @param[in] path Ruta del archivo donde se va a escribir.
     * @param[in] content Contenido a escribir en el archivo. Por defecto es cadena vacía.
     * @return true si se escribió correctamente, false en caso de error.
     */
    bool writeFile(const char *path, const String &content = "");

    /**
     * @brief Sobrecarga para escribir contenido en un archivo a partir de un String.
     * @param[in] path Ruta del archivo donde se va a escribir.
     * @param[in] content Contenido a escribir en el archivo. Por defecto es cadena vacía.
     * @return true si se escribió correctamente, false en caso de error.
     */
    bool writeFile(const String &path, const String &content = "");

    /*
     * @brief Lista los archivos y directorios dentro de una ruta dada.
     * @param[in] dirname Directorio que se listará.
     * @param[in] levels Niveles de profundidad para listar recursivamente.
     */
    //void listDir(const char *dirname, uint8_t levels);

    /**
     * @brief Crea un directorio en la tarjeta SD a partir de un String.
     * @param[in] path Ruta del directorio a crear.
     * @return true si se crea correctamente, false en caso de error.
     */
    bool createDir(const String &path);

    /**
     * @brief Crea un directorio en la tarjeta SD a partir de una cadena tipo C.
     * @param[in] path Ruta del directorio a crear.
     * @return true si se crea correctamente, false en caso de error.
     */
    bool createDir(const char *path);

    /**
     * @brief Elimina un directorio de la tarjeta SD.
     * @param[in] path Ruta del directorio a eliminar.
     * @return true si se elimina correctamente, false en caso de error.
     */
        bool removeDir(const char *path);

    /**
     * @brief Agrega contenido a un archivo existente (sin eliminar su contenido previo).
     * @param[in] path Ruta del archivo en el que se agrega contenido.
     * @param[in] content Contenido que será anexado en el archivo.
     * @return true si se anexa correctamente, false en caso de error.
     */
    bool appendFile(const char *path, String &content);

    /**
     * @brief Sobrecarga para anexar contenido a un archivo a partir de un String.
     * @param[in] path Ruta del archivo en el que se agrega contenido.
     * @param[in] content Contenido que será anexado en el archivo.
     * @return true si se anexa correctamente, false en caso de error.
     */
    bool appendFile(const String &path, String &content);

    /**
     * @brief Cambia el nombre de un archivo o lo mueve a otra ruta.
     * @param[in] path1 Ruta o nombre original del archivo.
     * @param[in] path2 Nueva ruta o nombre para el archivo.
     * @return true si se renombra correctamente, false en caso de error.
     */
    bool renameFile(const char *path1, const char *path2);

    /**
     * @brief Elimina un archivo de la tarjeta SD a partir de una cadena tipo C.
     * @param[in] path Ruta del archivo a eliminar.
     * @return true si se elimina correctamente, false en caso de error.
     */
    bool deleteFile(const char *path);

    /**
     * @brief Sobrecarga para eliminar un archivo a partir de un String.
     * @param[in] path Ruta del archivo a eliminar.
     * @return true si se elimina correctamente, false en caso de error.
     */
    bool deleteFile(const String& path);

    /**
     * @brief Elimina recursivamente un archivo o directorio de la tarjeta SD, incluyendo su contenido.
     * @param[in] path Ruta del archivo o directorio a eliminar.
     * @return true si se elimina correctamente, false en caso de error.
     */
    bool deleteRecursive(const String &path);
};

#endif // SD_CARD_MANAGER_H