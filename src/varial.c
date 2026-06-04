#include "varial.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#define RED "\x1b[1;31m"
#define RST "\x1b[0m"

void VarialSetDefaultConfig(VarialConfig *config) {
    config->map = "0123456789ABCDEF";
    config->seed = 0;
    config->buffer_size = 1048576;
    config->debug = false;
}

void VarialMapInitialize(unsigned char *lut, const char *map) {
    memset(lut, 0, 256);
    for (int i = 0; map[i] != '\0'; i++) lut[(unsigned char)map[i]] = (unsigned char)i;
}

int VarialEncode(int fd_in, int fd_out, VarialConfig config) {
    size_t map_len = strlen(config.map);
    unsigned char *ib = malloc(config.buffer_size);
    unsigned char *ob = malloc(config.buffer_size * 2);
    if (!ib || !ob) { fprintf(stderr, RED "E:" RST " Memory allocation failed!\n"); return -1; }
    ssize_t n;
    int rolling_seed = config.seed;
    while ((n = read(fd_in, ib, config.buffer_size)) > 0) {
        size_t j = 0;
        for (ssize_t i = 0; i < n; i++) {
            int shift = (rolling_seed++) % map_len;
            ob[j++] = config.map[(((ib[i] / map_len) + shift) % map_len)];
            ob[j++] = config.map[(((ib[i] % map_len) + shift) % map_len)];
        }
        write(fd_out, ob, j);
    }
    if (isatty(fd_out)) write(fd_out, "\n", 1);
    free(ib); free(ob);
    return 0;
}

int VarialDecode(int fd_in, int fd_out, VarialConfig config) {
    size_t map_len = strlen(config.map);
    unsigned char lut[256];
    VarialMapInitialize(lut, config.map);
    unsigned char *ib = malloc(config.buffer_size);
    unsigned char *ob = malloc(config.buffer_size / 2);
    if (!ib || !ob) { fprintf(stderr, RED "E:" RST " Memory allocation failed!\n"); return -1; }
    ssize_t n;
    int rolling_seed = config.seed;
    while ((n = read(fd_in, ib, config.buffer_size)) > 0) {
        size_t j = 0;
        for (ssize_t i = 0; i < n; i += 2) {
            int shift = (rolling_seed++) % map_len;
            unsigned char v1 = (lut[ib[i]] - shift + map_len) % map_len;
            unsigned char v2 = (lut[ib[i+1]] - shift + map_len) % map_len;
            ob[j++] = (v1 * map_len) + v2;
        }
        write(fd_out, ob, j);
    }
    if (isatty(fd_out)) write(fd_out, "\n", 1);
    free(ib); free(ob);
    return 0;
}

int main(int argc, char *argv[]) {
    int mode = 0, f_in = STDIN_FILENO, f_out = STDOUT_FILENO;
    VarialConfig config;
    VarialSetDefaultConfig(&config);
    char *in_f = NULL, *out_f = NULL;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-e") == 0) mode = 1;
        else if (strcmp(argv[i], "-d") == 0) mode = 2;
        else if (strcmp(argv[i], "-i") == 0 && i + 1 < argc) in_f = argv[++i];
        else if (strcmp(argv[i], "-o") == 0 && i + 1 < argc) out_f = argv[++i];
        else if (strcmp(argv[i], "-m") == 0 && i + 1 < argc) config.map = argv[++i];
        else if (strcmp(argv[i], "-s") == 0 && i + 1 < argc) config.seed = atoi(argv[++i]);
        else if (strcmp(argv[i], "-b") == 0 && i + 1 < argc) config.buffer_size = atol(argv[++i]);
        else if (argv[i][0] != '-') {
            if (!in_f) in_f = argv[i];
            else if (!out_f) out_f = argv[i];
        }
    }
    if (in_f) {
        f_in = open(in_f, O_RDONLY);
        if (f_in < 0) { fprintf(stderr, RED "E:" RST " File \"%s\" not found!\n", in_f); return 1; }
    }
    if (out_f) {
        f_out = open(out_f, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (f_out < 0) { fprintf(stderr, RED "E:" RST " Cannot create \"%s\"!\n", out_f); return 1; }
    }
    if (mode == 1) VarialEncode(f_in, f_out, config);
    else if (mode == 2) VarialDecode(f_in, f_out, config);
    if (f_in != STDIN_FILENO) close(f_in);
    if (f_out != STDOUT_FILENO) close(f_out);
    return 0;
}
