#ifndef _WIFI
#define _WIFI 1

extern bool configPortalenabled;

void setupWiFi();

void handleOTA();

void handleWiFi();

String getConfigParam(String name);

#endif