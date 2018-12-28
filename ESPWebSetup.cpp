/*
  ESPWebSetup.cpp - Library for ESP32 ssid and password setup mode over AP. 
  Created by Aleš Rosman, 26.12.2018
*/

#include "Arduino.h"
#include "ESPWebSetup.h"

// Local defines
#define ESP32_NAMESPACE       "ESP32ap"
#define ESP32_SSID_SETTING    "ssid"
#define ESP32_PASS_SETTING    "pass"
#define ESP32_MODE_SETTING    "mode"

ESPWebSetup::ESPWebSetup(char* ssid, char* pass)
{
  // create web server listening to port 80
  ESPWebSetup::webServer = new ESP32WebServer(80);
  AP_ssid = ssid;
  AP_pass = pass;
  currentMode = MODE_AP;
}

int ESPWebSetup::begin()
{
  // read preferences, and initialize variables
  ESPWebSetup::ESP32_NVMSettings.begin(ESP32_NAMESPACE, false);
  CL_ssid = ESPWebSetup::ESP32_NVMSettings.getString(ESP32_SSID_SETTING,"");
  CL_pass = ESPWebSetup::ESP32_NVMSettings.getString(ESP32_PASS_SETTING,"");
  currentMode = (ESP32_mode)ESPWebSetup::ESP32_NVMSettings.getUChar(ESP32_MODE_SETTING,MODE_AP);

  // which mode to run
  switch (currentMode)
  {
    case MODE_CLIENT:
      // Serial.println("MODE_CLIENT");
      runCL();
      break;
    default:
    case MODE_AP:
      // library handles AP mode with starting the web server
      // Serial.println("MODE_AP");
      runAP();
      break;
  }

  // return the mode, so the user knows what is going on
  return currentMode;
}

void ESPWebSetup::runCL()
{
  // try to connect to saved ssid, with saved password
  WiFi.begin(CL_ssid.c_str(), CL_pass.c_str());
  // wait for the connection
  int nTimeOutCnt = 0;
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    nTimeOutCnt++;
    if(nTimeOutCnt>120)
    {
      // can not connect, store AP mode and restart
      ESPWebSetup::ESP32_NVMSettings.putUChar(ESP32_MODE_SETTING,MODE_AP);
      ESP.restart();
    }
  }
  // connected, nothing else to do
}

void ESPWebSetup::runAP()
{
  // running AP mode web server
  WiFi.softAP(AP_ssid.c_str(), AP_pass.c_str());
  IPAddress AP_IP = WiFi.softAPIP();

  // register web server callback functions (using std:bind, to avoid static "mess")
  webServer->on("/",std::bind(&ESPWebSetup::handleAPRoot,this));
  webServer->onNotFound(std::bind(&ESPWebSetup::handleNotFound,this));
  webServer->on("/submit",HTTP_POST,std::bind(&ESPWebSetup::handleAPOnSubmit,this));

  // start web server
  webServer->begin();
}

int ESPWebSetup::update()
{
    // we only need to call handleClient in the main loop (and only if in AP mode)
    if (currentMode == MODE_CLIENT) webServer->handleClient();
}

// web server callbacks
void ESPWebSetup::handleAPRoot()
{
  webServer->send(200,"text/html",AP_WEB_PAGE_HTML);
}

void ESPWebSetup::handleAPOnSubmit()
{
  webServer->send(200, "text/plain", "Form submited! Restarting...");
  ESPWebSetup::ESP32_NVMSettings.putString(ESP32_SSID_SETTING,webServer->arg("ssid"));
  ESPWebSetup::ESP32_NVMSettings.putString(ESP32_PASS_SETTING,webServer->arg("pass"));
  ESPWebSetup::ESP32_NVMSettings.putUChar(ESP32_MODE_SETTING,MODE_CLIENT);
  ESP.restart();
}

void ESPWebSetup::handleNotFound()
{
  String message = "File Not Found\n\n";
  webServer->send(404, "text/plain", message);
}

