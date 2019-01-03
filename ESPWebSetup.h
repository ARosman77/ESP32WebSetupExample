/*
  ESPWebSetup.h - Library for ESP32 ssid and password setup mode over AP. 
  Created by Aleš Rosman, 26.12.2018
*/
#ifndef ESPWebSetup_h
#define ESPWebSetup_h

#include "Arduino.h"
#include <Preferences.h>
#include <WiFi.h>
#include <ESP32WebServer.h>

// Working modes 
enum ESP32_mode_t { MODE_AP, MODE_CLIENT };

class ESPWebSetup
{
  public:

    // Main functionality
    ESPWebSetup(char* ssid, char* pass);
    ESP32_mode_t begin();
    void update();

    // support functions
    ESP32_mode_t getCurrentMode();
    IPAddress getIPAddress();
    
  
  private:
  ESP32_mode_t currentMode;
  
  // Preferences = NVM storage
  const char *ESP32_NAMESPACE =     "ESP32ap";
  const char *ESP32_SSID_SETTING =  "ssis";
  const char *ESP32_PASS_SETTING =  "pass";
  const char *ESP32_MODE_SETTING =  "mode";
  Preferences ESP32_NVMSettings;

  // AP mode ssid and password
  String AP_ssid;
  String AP_pass;

  // CL mode ssid and password for WIFI client
  String CL_ssid;
  String CL_pass;

  // web server
  ESP32WebServer *webServer;

  // IP address of web server
  IPAddress _IP_Address;

  void handleAPRoot();
  void handleAPOnSubmit();
  void handleNotFound();

  // run in client mode
  void runCL();
  // run in AP mode
  void runAP();

  // homepage for AP, to be defined correctly later
  // simple web page with forms
  const char *AP_WEB_PAGE_HTML = 
  "<!DOCTYPE html>\n"
  "<html>\n"
  "<body>\n"
  "<h1 style=\"color:blue;margin-left:30px;\">\n"
  "\tESP32 WiFi Settings\n"
  "</h1>\n"
  "<form action=\"/submit\" method=\"post\">\n"
  "    <table>\n"
  "    <tr>\n"
  "    <td><label class=\"label\">Network Name : </label></td>\n"
  "    <td><input type = \"text\" name = \"ssid\"/></td>\n"
  "    </tr>\n"
  "    <tr>\n"
  "    <td><label>Password : </label></td>\n"
  "    <td><input type = \"text\" name = \"pass\"/></td>\n"
  "    </tr>\n"
  "    <tr>\n"
  "    <td align=\"center\" colspan=\"2\"><input style=\"color:blue;margin-left:auto;margin-right:auto;\" type=\"submit\" value=\"Submit\"></td>\n"
  "    </tr>\n"
  "    </table>\n"
  "</form>\n"
  "</body>\n"
  "</html>";
  
};

#endif
