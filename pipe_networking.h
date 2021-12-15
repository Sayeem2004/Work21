#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#ifndef NETWORKING_H
#define NETWORKING_H
#define ACK "HOLA"
#define WKP "mario"

#define HANDSHAKE_BUFFER_SIZE 10
#define BUFFER_SIZE 1000

int server_handshake(int *to_client);
int client_handshake(int *to_server);
void print_error(int err, char *msg);

#endif
