#include <RestClient.h>

#include "config.h"
#include "utils.h"
#include "confstore.h"

// RestClient client(spoolman_host.c_str(), spoolman_port, spoolman_protocol.c_str(), spoolman_api_endpoint.c_str());

void requestSpool(int spoolId)
{
    RestClient client(spoolman_host, spoolman_port, spoolman_protocol, spoolman_api_endpoint);

    String response;
    String endpoint = String("spool/") + String(spoolId, DEC);
    if (client.get(endpoint, &response) == 200) {
        debug_println("*HTTP: Got response from SpoolMan.");
        Serial.println(response);
    } else {
        Serial.println("*HTTP: Error requesting spool from remote...");
    }
}