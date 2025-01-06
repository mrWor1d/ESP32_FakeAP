# FakeAPlib

Librerría personal donde se incluirán todas las dependecias y librerías externas para el desarollo de un punto de acceso falso con un`ESP32-WROVER-E`.<br>

## Como usar

Se tendrá que descargar esta librería y añadirla a `Arduino IDE` como una librería externa. Una vez hecho eso, se podrá usar en cualquier proyecto arduino en el equipo.<br>
Ruta para añadir librerías externas: ``Sketch -> Include library -> Add .ZIP  Library...``

![Screenshot of a the route to add external libraries in Arduino IDE](image.png)

Encontrarán ejemplos de implementación en la carpeta [examples](./examples/).

## Dependencias

Las siguientes librerías se tienen que descargar previamente a la compilicación del cógigo:
* WiFi.h
* WiFiAP.h
* WebServer.h
* DNSServer.h
* SD_MMC.h
* FS.h