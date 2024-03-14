#ifndef SCALE_H
#define SCALE_H 1

extern bool has_scale;
extern long current_weight;
extern long scale_offset;

bool setupScale();

void handleScale();

#endif