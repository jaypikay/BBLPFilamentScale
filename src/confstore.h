#ifndef _CONFSTORE
#define _CONFSTORE 1

extern String mqtt_host;
extern uint16_t mqtt_port;
extern String mqtt_username;
extern String mqtt_password;

extern String spoolman_host;
extern String spoolman_protocol;
extern uint16_t spoolman_port;
extern String spoolman_api_endpoint;
extern bool spoolman_use_https;

void setupConfigFS();

bool loadConfig();
bool saveConfig();

#endif