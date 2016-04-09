/*
 * IoF_WiFiClient.cpp
 *
 *  Created on: 08.04.2016
 *      Author: scan
 */

#include <iof_WiFiClient.h>

#include <Timer.h>


//-----------------------------------------------------------------------------
// WiFi Connect Timer Adapter
//-----------------------------------------------------------------------------

class MyWifiConnectTimerAdapter : public TimerAdapter
{
private:
  IoF_WiFiClient* m_wifiClient;
  wl_status_t m_wlStatus;

public:
  MyWifiConnectTimerAdapter(IoF_WiFiClient* wifiClient)
  : m_wifiClient(wifiClient)
  , m_wlStatus(WL_NO_SHIELD)
  { }

  void timeExpired()
  {
    m_wifiClient->printWiFiStatusChanged(m_wlStatus);
  }
};



//-----------------------------------------------------------------------------
// IoF WiFi Client
//-----------------------------------------------------------------------------

IoF_WiFiClient::IoF_WiFiClient(char* wifi_ssid, char* wifi_pw, unsigned long connectInterval_ms)
: m_wifiConnectTimer(0)
, m_WiFi_ssid(wifi_ssid)
, m_WiFi_pw(wifi_pw)
, m_connectInterval_ms(connectInterval_ms)
, m_client(new WiFiClient())
{ }

IoF_WiFiClient::~IoF_WiFiClient()
{
  delete m_wifiConnectTimer;
  m_wifiConnectTimer = 0;
  delete m_client;
  m_client = 0;
}

wl_status_t IoF_WiFiClient::getStatus()
{
  return WiFi.status();
}

void IoF_WiFiClient::begin()
{
  WiFi.begin(m_WiFi_ssid, m_WiFi_pw);
  m_wifiConnectTimer = new Timer(new MyWifiConnectTimerAdapter(this), Timer::IS_RECURRING, m_connectInterval_ms);
}

void IoF_WiFiClient::printWiFiStatusChanged(wl_status_t& old_wlStatus)
{
  wl_status_t wlStatus = getStatus();
  if (wlStatus != old_wlStatus)
  {
    old_wlStatus = wlStatus;
    Serial.print("WiFi - SSID: ");
    Serial.print(m_WiFi_ssid);
    Serial.print(" - Status: ");
    Serial.println(wlStatus == WL_NO_SHIELD       ? "NO_SHIELD      " :
                    wlStatus == WL_IDLE_STATUS     ? "IDLE_STATUS    " :
                    wlStatus == WL_NO_SSID_AVAIL   ? "NO_SSID_AVAIL  " :
                    wlStatus == WL_SCAN_COMPLETED  ? "SCAN_COMPLETED " :
                    wlStatus == WL_CONNECTED       ? "CONNECTED      " :
                    wlStatus == WL_CONNECT_FAILED  ? "CONNECT_FAILED " :
                    wlStatus == WL_CONNECTION_LOST ? "CONNECTION_LOST" :
                    wlStatus == WL_DISCONNECTED    ? "DISCONNECTED   " : "UNKNOWN");
  }
}

