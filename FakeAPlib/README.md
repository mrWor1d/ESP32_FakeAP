# FakeAPlib

Librerría personal donde se incluirán todas las dependecias y librerías externas para el desarollo de un punto de acceso falso con un`ESP32-WROVER-E`.<br>

Les invitamos a consultar la [documentación](./documentation/html/index.html) del código completo para obtener más detalles sobre el mismo.<br>


## Configurar el entorno

Se tendrá que descargar esta librería y añadirla a `Arduino IDE` como una librería externa. Una vez hecho eso, se podrá usar en cualquier proyecto arduino en el equipo.<br>
Ruta para añadir librerías externas: ``Sketch -> Include library -> Add .ZIP  Library...``

![Screenshot of a the route to add external libraries in Arduino IDE](image.png)

Encontrarán en el siguiente enlace los pasos detallados para poder hacerlo: [Cómo instalar una librería de Arduino en el entorno de desarrollo](https://programarfacil.com/blog/arduino-blog/instalar-una-libreria-de-arduino/)

Encontrarán ejemplos de uso de esta libreria en la carpeta [examples](./examples/).<br>


## Dependencias
Las siguientes librerías se tienen que descargar previamente a la compilicación del cógigo:
* SD_MMC.h
* FS.h
* UrlEncode.h

Se han selecionado librerias que están disponibles en el gestor de librería del `Arduino IDE`. Solo se tendrán que buscar y descargar.