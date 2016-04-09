/*
 * IoF_WiFiClient.h
 *
 *  Created on: 08.04.2016
 *      Author: scan
 */

#ifndef PROD_SOCKETS_IOF_WIFICLIENT_H_
#define PROD_SOCKETS_IOF_WIFICLIENT_H_

#include <WiFiClient.h>

#include <ESP8266WiFi.h>

class Timer;

class IoF_WiFiClient
{
public:
  IoF_WiFiClient(char* wifi_ssid, char* wifi_pw, unsigned long connectInterval_ms);
  virtual ~IoF_WiFiClient();

  void begin();
  void printWiFiStatusChanged(wl_status_t& old_wlStatus);
  wl_status_t getStatus();
  inline WiFiClient* getClient() { return m_client; };

private:
  Timer* m_wifiConnectTimer;
  char* m_WiFi_ssid;
  char* m_WiFi_pw;
  unsigned long m_connectInterval_ms;
  WiFiClient* m_client;

private:  // forbidden functions
  IoF_WiFiClient(const IoF_WiFiClient& src);              // copy constructor
  IoF_WiFiClient& operator = (const IoF_WiFiClient& src); // assignment operator
};

#endif /* PROD_SOCKETS_IOF_WIFICLIENT_H_ */
