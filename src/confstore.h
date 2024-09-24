#ifndef _CONFSTORE
#define _CONFSTORE 1

extern String mqtt_host;
extern uint16_t mqtt_port;
extern String mqtt_username;
extern String mqtt_password;

extern String spoolman_host;
extern uint16_t spoolman_port;
extern String spoolman_protocol;
extern String spoolman_api_endpoint;

void setupConfigFS();

bool loadConfig();
#ifdef ENABLE_DEBUG
void dumpConfigFile();
#endif
bool saveConfig();

#endif
