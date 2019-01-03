/*
  ESPWebSetup.cpp - Library for ESP32 ssid and password setup mode over AP. 
  Created by Aleš Rosman, 26.12.2018
*/

#include "Arduino.h"
#include "ESPWebSetup.h"

// Local defines
#define _DEBUG          // for debugging only

ESPWebSetup::ESPWebSetup(char* ssid, char* pass)
{
  // create web server listening to port 80
  webServer = new ESP32WebServer(80);
  AP_ssid = ssid;
  AP_pass = pass;
  currentMode = MODE_AP;
}

ESP32_mode_t ESPWebSetup::begin()
{

#ifdef _DEBUG
  Serial.begin(115200);
  Serial.println("Reading preferences: ");
#endif
  
  // read preferences, and initialize variables
  ESP32_NVMSettings.begin(ESP32_NAMESPACE, false);
  CL_ssid = ESP32_NVMSettings.getString(ESP32_SSID_SETTING,"");
  CL_pass = ESP32_NVMSettings.getString(ESP32_PASS_SETTING,"");
  currentMode = (ESP32_mode_t)ESP32_NVMSettings.getUChar(ESP32_MODE_SETTING,MODE_AP);

#ifdef _DEBUG
  Serial.print("  ssid: ");
  Serial.println(CL_ssid);
  Serial.print("  pass: ");
  Serial.println(CL_pass);
#endif

  // which mode to run
  switch (currentMode)
  {
    case MODE_CLIENT:
    #ifdef _DEBUG
      Serial.println("Running MODE_CLIENT!");
    #endif
      runCL();
      break;
    default:
    case MODE_AP:
      // library handles AP mode with starting the web server
    #ifdef _DEBUG
      Serial.println("Running MODE_AP!");
    #endif
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
#ifdef _DEBUG
  Serial.print("Trying to connect to ");
  Serial.print(CL_ssid);
  Serial.println(" ...");
#endif
  // wait for the connection
  int nTimeOutCnt = 0;
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    nTimeOutCnt++;
    if(nTimeOutCnt>120)
    {
      // can not connect, store AP mode and restart
      ESP32_NVMSettings.putUChar(ESP32_MODE_SETTING,MODE_AP);
      ESP.restart();
    }
  }
  // connected, store ip address
  _IP_Address = WiFi.localIP();
#ifdef _DEBUG
  Serial.print("Connected to ");
  Serial.print(CL_ssid);
  Serial.print(" @ ");
  Serial.print(_IP_Address);
  Serial.println(" !");
#endif
}

void ESPWebSetup::runAP()
{
  // running AP mode web server
  WiFi.softAP(AP_ssid.c_str(), AP_pass.c_str());
  _IP_Address = WiFi.softAPIP();

#ifdef _DEBUG
  Serial.print("Starting AP at ");
  Serial.print(_IP_Address);
  Serial.println(" ...");
#endif

  // register web server callback functions (using std:bind, to avoid static "mess")
  webServer->on("/",std::bind(&ESPWebSetup::handleAPRoot,this));
  webServer->onNotFound(std::bind(&ESPWebSetup::handleNotFound,this));
  webServer->on("/submit",HTTP_POST,std::bind(&ESPWebSetup::handleAPOnSubmit,this));

  // start web server
  webServer->begin();
}

void ESPWebSetup::update()
{
    // we only need to call handleClient in the main loop (and only if in AP mode)
    if (currentMode == MODE_AP) webServer->handleClient();
}

// web server callbacks
void ESPWebSetup::handleAPRoot()
{
  webServer->send(200,"text/html",AP_WEB_PAGE_HTML);
}

void ESPWebSetup::handleAPOnSubmit()
{
  webServer->send(200, "text/plain", "Form submited! Restarting...");
  ESP32_NVMSettings.putString(ESP32_SSID_SETTING,webServer->arg("ssid"));
  ESP32_NVMSettings.putString(ESP32_PASS_SETTING,webServer->arg("pass"));
  ESP32_NVMSettings.putUChar(ESP32_MODE_SETTING,MODE_CLIENT);
  ESP.restart();
}

void ESPWebSetup::handleNotFound()
{
  String message = "File Not Found\n\n";
  webServer->send(404, "text/plain", message);
}

ESP32_mode_t ESPWebSetup::getCurrentMode()
{
  return currentMode;
}

IPAddress ESPWebSetup::getIPAddress()
{
  return _IP_Address;
}

