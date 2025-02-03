# ESP32_FakeAP
Esto es un proyecto escolar que tiene como objetivo el descubrimiento y el uso de las funcionalidades de la placa `ESP32`.
<br>


## Tabla de contenido

- [Descripción](#descripción)
    * [Estructura](#estructura)
- [Componentes](#componentes)
    * [Físicos](#físicos)
    * [Librerias personales](#librerias-personales)
    * [Librerias externas](#librerias-externas)
- [Fuentes y recursos adicionales](#fuentes-y-recursos-adicionales)
- [Arbol del repositorio](#project-tree)
<br>


## Descripción
Este código sirve tanto para crear un `punto de acceso falso` (Fake Access Point) como servidor web usando la placa `ESP32`.<br>
Se trata de un _wifi_ público, que pedirá al usuario de conectarse a su cuenta en una de sus redes (`Instagram`, `Facebook`, `X`... etc.) para poder usar la red de wifi.<br>
Los datos proporcionados serán transferidos a una segunda placa que actuará como _servidor_, igualmente implementado con una placa `ESP32`, donde se almacenarán en una _tarjeta SD_.<br>
El Servidor tiene diferentes funcionalidades como:
* Proporciona información de la red y del almacenamiento en tiempo real
* Gestión de archivos (carga/descarga, borrar)
* Gestión de algunas configuración basicas como conexión wifi y rutas de algunos archivos
<br>

> [!CAUTION]
> Esto es únicamente a fines educativos. No se hará ningún uso ilícito de este código.
<br>

### Estructura
Para facilitar el mantenimiento, la legibilidad y la reusabilidad del código, se desarolló una librería que solo se tendrá que añadir a `Arduino IDE` y usarla en su sketch.
* Los archivos fuentes de la librería se encontrarán en la carpet [FakeAPlib](FakeAPlib). Su documentación se puede encontrar en:
    - [index.html](FakeAPlib/documentation/html/index.html): documentación en formato HTML
    - [doc.chm](FakeAPlib/documentation/html/doc.chm): documentación en formato HTML compilado (Windows)
    - [refman.pdf](FakeAPlib/documentation/latex/refman.pdf): documentación en formato PDF
* Se encontrará el código Arduino en las carpetas:
    - [ESP32_honeyPot](ESP32_honeyPot): código a ejecutar en la placa del punto de acceso
    - [ESP32_Server](ESP32_Server): código a ejecutar en la placa del servidor
* Hay ejemplos de uso en la carpeta [examples](FakeAPlib/examples/)
* Todas las rutas web están recogidas en el archivo [IP_Routes.txt](IP_Routes.txt)
* Codigo fuente de las páginas html:
    - [index.html](webpages/index.html): página de inicio del punto de acceso cautivo
    - [index-styles.css](webpages/index-styles.css): para el formato de la página [index.html](webpages/index.html) 
    - [application.js](webpages/application.js): lógica de la página [index.html](webpages/index.html)
    - [icons](webpages/icons): aquí se encontrarán todas las imagenes de las paginas
    - [admin-login](webpages/admin-login): aquí se encontrarán todos los archivos relativos la página de administrador
    > Las carpetas con un nombre `****-login`contienen los códigos de las páginas de redes sociales. Consultar las fuentes [aquí](#fuentes-y-recursos-adicionales)

<img height= 50 src="/webpages/icons/icons8-facebook-48.svg" width=50/> <img height= 50 src="/webpages/icons/icons8-instagram-48.svg" width=50/> <img height= 50 src="/webpages/icons/icons8-twitterx-48.svg" width=50/> <img height= 50 src="/webpages/icons/icons8-google-48.svg" width=50/>

> Pueden consultar el arbol del proyecto [aquí](#project-tree)
<br>


## Componentes

### Físicos
* Dos placas ESP32 (un punto de acceso y un servidor)
> [!IMPORTANT]
> La placa debe de tener un lector de tarjeta integrado. Utilizamos placas ESP32-WROVER-E,
> pero recomendamos comprar una `ESP32-CAM` o `ESP32-WROVER-IE` que viene con conector para antena wifi,
> aunque debería valer cualquiera placa con lector tarjeta integrado. 
* Tarjeta SD formateada previamente con el sistemas de archivos `FAT32`
> [!IMPORTANT]
> La tarjeta SD no tiene que superar los 32GB para poder aplicar el formato `FAT32`.
* Dos modulos de batería para alimentar las placas<br>

| Componentes           | Precios         |
| --------------------- | :-------------: |
| Placa ESP32-WROVER-IE | [Amazon: €13,66](https://www.amazon.es/Bntaomle-ESP-WROVER-IE-m%C3%B3dulo-compatible-Bluetooth/dp/B0D66BQZKK?__mk_es_ES=%C3%85M%C3%85%C5%BD%C3%95%C3%91&crid=3M1T9TWQKJUNK&dib=eyJ2IjoiMSJ9.Pb0cAGviFdOSi3v84s7akwOnx7sKWzlnBhYTHcCDqVR3OxlS1EEDuvc2YA_Wo3Oh5kR-052_q3KQ0lNmy9g0dyzqtMyX9HhZ_1TzSu0h7nA64WtvWIU4fjKRap6ljonh2aMsM_LhEZWBp4WdblGNYFWowmUQay4q9k5wBpvSFPGsD5svMvxTB7Lr1bSHG6_eniW1g2FredeYiblAVeCcq3fDcNQ3efAfi6I7EOngIvVOedbnUPc1gqbdVjNq1Rh10rraIxdD54X0nQ1Q_795rza38JctosvcAFCd6ijPAY71wk6u1er18HT8TEQ75IMAegOKwvlyoBo1yDkM2_Vcw6fF1K1QPdVixnPUVHgJfuBlj1zzNdVE6fpGEYg1MiuPjJCYPp-vFe9_kFivbwTaa-dRPJo-KpT7pWvPaAZdokb1aPaole_YYdGpE6I4etlA.zXrpyd2hRQ1Lj4xGr9h8IWQo1Htwucq5WqkGge9whPg&dib_tag=se&keywords=esp32+wrover+ie&nsdOptOutParam=true&qid=1736037259&sprefix=esp32+wrover+ie%2Caps%2C91&sr=8-14&xpid=gSSBBdjGdvRr2)  |
| Placa ESP32-CAM       | [Amazon: €11,99](https://www.amazon.es/Diymore-ESP32-CAM-Desarrollo-Bluetooth/dp/B0D9BSKMBZ?__mk_es_ES=%C3%85M%C3%85%C5%BD%C3%95%C3%91&crid=20MNI754JV3MU&dib=eyJ2IjoiMSJ9.XTYDyktoNJJQo27qurFwQgUaTchKbmXDsYQOVjH80Yg4MDLIVxHD75MJLbfWHcOh7bJ8RIpPp0i2Jt18U0zI12S3VtFuJ7MqGUm8I9TWV-cVDAYcbI8ActKSgSi6QWYUc2KwpkfQ2tYl6Pp0S2EVoqd-g5a0t7dh3MRRNa3BvzisxxPrUcMUMldWy4OD_LEP5HUQzEJOI3vm_pM_s-1amJQQaFYpN5ou0aLMA0Y-abxOuPKrI2TE_4wIV6iWqNphjtTfWx0kyo5O4MMSs4SdSAM3EPm7ts1EiTWWjC8GBajekXyxfOGEGYClr4tMfyhNoYogDqAr47aWrs2dwLath6FpYT14V_AiQCfZsznthzeIAi0_4SvB9kEJV4JYgSNd_iPtZu-rqfG9tbsR9KOysLJCDHg6Np5UBzYGND-cuX3nMHC3n6a_d138u1G-MLq-.MIyyr5C4APQu2NxGXUvHE_KBpERaPZB762ci9pbz8Ow&dib_tag=se&keywords=esp32+wrover&nsdOptOutParam=true&qid=1736036443&sprefix=esp32+wrover%2Caps%2C110&sr=8-23)  |
| Tarjeta SD 16GB       | [Amazon: €6,87](https://www.amazon.es/Kodak-Premiun-SDHC-16GB-Class10/dp/B01FZPDRA4?crid=3GR8LY1LWAOZY&dib=eyJ2IjoiMSJ9.0kLS3uvwbDCbwpvUcLi5-GgV6qAhXJurERZ55fT5tsRVaWdKkBz8PBrufeMOG_n89iCzgs5lU-cKcK7LdGVTP7QoN8m0KRlSND5NtChLQVSwZMEre9nVGzciLRK9ZmK5cDZY6c2R1Mefm9PcYcA3No2ft_BvC9jvgm_R95HEGPTLf1Ga0f0w3Etd15N44nR_uzluLcg1sRY95fDxVL5vhl_X9gY5ngWXZ8cF_ZhIniMLmBDcYMQRUleSTJ0FIdlldERwNbAMdsGwvOjEbhxqNFN_KyGtno4zjC4wcS7to0bJZedjdspB2a2M1C19Bo9508gKbQGnQmKvnh_0EgNqLNY6FgImuvQOkseJd_f6mJoexv_oaCA0ms5GueJ4uwuxwafy-QntUpDJGLCKwSynYxZmMNmRbsBIN1MTPhur1b9_saTEsfqMM1YuM29q1QGA.i422wxWCoWfc2CY5KTSR-cOouiKQLb98eExASJJe81o&dib_tag=se&keywords=tarjeta+sd+16gb&nsdOptOutParam=true&qid=1736031694&sprefix=tarjeta+sd+16%2Caps%2C144&sr=8-5)  |
| Tarjeta SD 32GB       | [Amazon: €7,95](https://www.amazon.es/SANDISK-Ultra-MICROSDHC-Adapter-100MB/dp/B08HVWJM8C?dib=eyJ2IjoiMSJ9.R1o3XhQ4AKSpUNWJUASxGu26T3mnxr_wq-O8EaxKUiiaENOTRmQl9wKTn6Tx3Rf5d1TF3lsJ1qa8LcuKelgDXxepsy31Xj6Cw6HuO26xTU8rJx_sOviRCI-DqNyP7nhsB2__ULdgaewxx4U5eRj6rNtz5i1ad7WtTSS2hj0AVG0m7CFLojaRciwtYk3CLJ2OvSBhF8slD8HQvtu3ITXEtq77VC0LvK_dhdy0FxkFhwHoa55MjsvPSmpLNQ-bWpyp_lOyHe2Q0VXUAZdYWJ9bixzbmMdNnFrF5TDxuTfxZulGmFdnlQVRYhnvBJjO-8TGLVP2G048iv2aijloUGEEjtf2sYHV6_T8XlrDCcUkDhW2zuu7Vn6tOMzSBTWMF8u1-aTEYwe-uMoKqHe_JdlgtCcm2_Epe08UdblcHYFSxhnPktSxD6x_yg1G9mgjMopR.3Kp6jRwcV8Ptb21pfHsxvkV3GngDWPLAE10L3KQwY58&dib_tag=se&keywords=tarjeta+sd+32gb&nsdOptOutParam=true&qid=1736030740&sr=8-25)  |
| Antena wiFi           | [Amazon: €9,90](https://www.amazon.es/KHIYQILO-Antena-Antenas-Coaxial-Tarjeta/dp/B0D9R4ZB17/ref=sr_1_6?crid=30GWSEYC4XMYG&dib=eyJ2IjoiMSJ9.4Xi05MIvmhvy_w6iaYr_h5PpCkLNLhgZ_wMGZ96SoUTkoQ0ZsXR5E0xm0Dfu_NOPPexMdi0ueSaDTXtyJ0ddKu6YAStUhImo_6_oPOFO6rmnTt3sb7yVBUH3X67uOosLKZZHs_2o6TCNU0Os84OjG6wpSchwG002mXY2H_AhoRT1ktiIxiC2VBvBGXAyxcg2lQeXdKkuVfHgRhCEPJeM7Lscb9pCR7EVfKwWZxkIzw6ydUqfe2Pokgi1fUiRXrZnTScA3OwxadelvjBhGyVy48OgyUIzEG0lCkWRGGQslUQWiY9XbprDcBlQwhBNlM5tJmC5sFAs943E1A_aI4z9OlcYNEidgvlNuZPWiRHjYXXZtsnHC7kW4TTwvz0sZiXP7U7yVTzF6tCXvR5kgiSvWb252bXkPyU4OKKSaASewI9RdwUxAJUAHfsxG4KiURVy.lo6D9EaAeIW0rAPwXzS1c-PUL005NznodKXGVJBq54I&dib_tag=se&keywords=esp32+wifi+antena&qid=1737835764&sprefix=esp32+wifi+%2Caps%2C183&sr=8-6)  |
| Modulo de baterías    | [Amazon: €8,99](https://www.amazon.es/dp/B0CSJYM49V?asc_campaign=8fe350f5b3749f18898fae0ca2b28fd1&asc_source=01H1P39M5ZSG9J6WR6B1HBK9M0&newOGT=1&ref=cm_sw_r_cso_wa_apin_dp_W3N17E5XA01VXM0R38XN&ref_=cm_sw_r_cso_wa_apin_dp_W3N17E5XA01VXM0R38XN&skipTwisterOG=1&social_share=cm_sw_r_cso_wa_apin_dp_W3N17E5XA01VXM0R38XN&starsLeft=1&tag=snxes153-21)  |
<br>

### Librerias personales
Se tendrá que exportar la carpeta [FakeAPlib](FakeAPlib) como `.zip`, y añardirla en `Arduino IDE` como una librería.<br>
* FakeAPlib.h
* SDCardManager.h
* WebServerHandler.h
* WifiCaptivePortal.h
* include.h
> [!NOTE]
> Pueden consultar el [README](FakeAPlib/README.md) de la librería para obtener más detalles sobre la instalación de librerías externas, y sobre el código fuente.
<br>

### Librerias externas
* WiFi.h
* WiFiAP.h
* WiFiSTA.h
* WebSever.h
* DNSServer.h
* NetworkClient.h
* SD_MMC.h
* FS.h
* String
<br>


## Fuentes y recursos adicionales

| Archivos                              | Fuente                 |
| ------------------------------------- | ---------------------- |
| Páginas de inicio de sesión facebook  | [geeksforgeeks.org](https://www.geeksforgeeks.org/how-to-create-facebook-login-page-in-html-css/) |
| Páginas de inicio de sesión google    | [ziontutorial.com](https://ziontutorial.com/how-to-make-google-login-form-design-using-html-css/) |
| Páginas de inicio de sesión instagram | [Github repository](https://github.com/codingmarket07/Instagram-Login-Page) |
| Páginas de inicio de sesión twitter   | [geeksforgeeks.org](https://www.geeksforgeeks.org/how-to-create-twitter-login-page-using-html-css-javascript/) |
| Icono Facebook                        | [icons8.com](https://icons8.com/icons/set/facebook-logo) |
| Icono Google                          | [icons8.com](https://icons8.com/icons/set/google) |
| Icono Instagram                       | [icons8.com](https://icons8.com/icons/set/instagram-logo) |
| Icono Twitter                         | [icons8.com](https://icons8.com/icons/set/twitterx-logo) |
| Icono Wifi                            | [icons8.com](https://icons8.com/icons/set/wifi-logo) |


<br>

| Recursos adicionales                                  |
| ----------------------------------------------------- |
| [Proyecto similar de punto de acceso falso](https://github.com/TheKevinWang/ESP32FakeAP) |
| [Proyecto similar de servidor web](https://github.com/bdash9/ESP32-FileServer-HTTP-HTTPS) |
| [Ejemplo de comunicación entre dos placas por wifi](https://esp32io.com/tutorials/communication-between-two-esp32) |
| [Ejemplo de wifi como punto acceso y estación](https://randomnerdtutorials.com/esp32-useful-wi-fi-functions-arduino/) |
| [Ejemplo de servidor web](https://github.com/ESP32-Work/ESP32_Webserver/tree/main/ESP32_WebServer) |
| [Librerías ESPRESSIF arduino-esp32](https://github.com/espressif/arduino-esp32/tree/master/libraries) |
| [Documentación ESPRESSIF](https://docs.espressif.com/projects/arduino-esp32/en/latest/api/wifi.html) |
| [Documentación Arduino](https://docs.arduino.cc/language-reference/#functions) |

<br><br>

<a name="project-tree"></a> 
````
ESP32_FakeAP/
│
├── ESP32_honeyPot/
│   └── ESP32_honeyPot.ino
│
├── ESP32_Server/
│   └── ESP32_Server.ino
│
├── FakeAPlib/
│   ├── examples/
│   │   ├── fake_access_point/
│   │   │   └── fake_access_point.ino
│   │   │
│   │   └── web_server/
│   │       └── web_server.ino
│   │
│   ├── src/
│   │   ├── documentation/
│   │   │   ├── html/
│   │   │   └── latex/
│   │   │
│   │   ├── include/
│   │   │   ├── include.h
│   │   │   ├── SDCardManager.cpp
│   │   │   ├── SDCardManager.h
│   │   │   ├── WebServerHandler.cpp
│   │   │   ├── WebServerHandler.h
│   │   │   ├── WifiCaptivePortal.cpp
│   │   │   └── WifiCaptivePortal.h
│   │   │
│   │   ├── FakeAPlib.cpp
│   │   └── FakeAPlib.h
│   │
│   ├── .gitignore
│   ├── docu
│   ├── image.png
│   ├── keywords.txt
│   ├── library.properties
│   └── README.md
│ 
├── webpages/
│   ├── admin-login/
│   │   ├── admin-app.js
│   │   ├── admin-panel.html
│   │   ├── admin-style.css
│   │   ├── index-app.js
│   │   ├── index-login.html
│   │   └── index-styles.css
│   │
│   ├── facebook-login/
│   │   ├── index.html
│   │   └── styles.css
│   │
│   ├── google-login/
│   │   ├── index.html
│   │   └── styles.css
│   │
│   ├── icons/
│   │   ├── appstore.png
│   │   ├── facebook.png
│   │   ├── googlepay.png
│   │   ├── icons8-facebook-48.svg
│   │   ├── icons8-google-48.svg
│   │   ├── icons8-instagram-48.svg
│   │   ├── icons8-twitterx-48.svg
│   │   ├── instagram.png
│   │   ├── instagram-icon.svg
│   │   ├── public-wifi-5403_128.gif
│   │   └── X_logo_2023_original.svg
│   │
│   ├── instagram-login/
│   │   ├── index.html
│   │   └── styles.css
│   │
│   ├── twitter-login/
│   │   ├── index.html
│   │   └── styles.css
│   │
│   ├── application.js
│   ├── index.html
│   └── index-styles.css
│
├── .gitignore
├── IP_Routes.txt
├── LICENSE
└── README.md

````