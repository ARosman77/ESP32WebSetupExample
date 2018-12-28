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

class ESPWebSetup
{
  public:
    ESPWebSetup(char* ssid, char* pass);
    int begin();
    int update();
  
  private:
  
  // Working modes 
  enum ESP32_mode { MODE_AP, MODE_CLIENT } currentMode;

  // Preferences = NVM storage
  static Preferences ESP32_NVMSettings;
  const char *ESP32_namespace = "ESP32ap";
  const char *ESP32_ssid_setting = "ssis";
  const char *ESP32_pass_setting = "pass";
  const char *ESP32_mode_setting = "mode";

  // AP mode ssid and password
  String AP_ssid;
  String AP_pass;

  // CL mode ssid and password for WIFI client
  String CL_ssid;
  String CL_pass;

  // web server
  static ESP32WebServer *webServer;

  static void handleAPRoot();
  static void handleAPOnSubmit();
  static void handleNotFound();

  // run in client mode
  void runCL();
  // run in AP mode
  void runAP();

  // homepage for AP, to be defined correctly later
  // simple web page with forms
  static constexpr char *AP_WEB_PAGE_HTML = 
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
