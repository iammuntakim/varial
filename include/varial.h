#ifndef VARIAL_H
#define VARIAL_H

#include <stddef.h>
#include <stdbool.h>

typedef struct {
    char *map;
    int seed;
    size_t buffer_size;
    bool debug;
} VarialConfig;

int VarialEncode(int fd_in, int fd_out, VarialConfig config);
int VarialDecode(int fd_in, int fd_out, VarialConfig config);
void VarialMapInitialize(unsigned char *lut, const char *map);
void VarialSetDefaultConfig(VarialConfig *config);

#endif
