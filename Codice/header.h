#ifndef HEADER_H
#define HEADER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <netinet/in.h>

// Costanti
#define MAX_POSTI 5      
#define PUB_PORT 8080
#define CAMERIERE_PORT 8081
#define BUFFER_SIZE 1024

// Strutture
typedef struct {
    int socket_fd;
    struct sockaddr_in address;
} Client;

typedef struct {
    int posti_liberi;
    int posti[MAX_POSTI]; 
    pthread_mutex_t mutex;
} Pub;

// Funzioni generiche
void error(const char *msg);
void send_message(int socket, const char *message);
int receive_message(int socket, char *buffer);
void mostra_menu();


#endif
