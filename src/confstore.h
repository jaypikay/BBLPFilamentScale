#ifndef _CONFSTORE
#define _CONFSTORE 1

extern String mqtt_host;
extern uint16_t mqtt_port;
extern String mqtt_username;
extern String mqtt_password;

void setupConfigFS();

bool loadConfig();
bool saveConfig();

#endif