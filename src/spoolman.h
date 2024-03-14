#ifndef SPOOLMAN_H
#define SPOOLMAN_H

typedef struct {
    char deviceId[16];
    bool webPortal;
} SpoolmanStatus_t;

extern SpoolmanStatus_t spoolmanStatus;

#endif