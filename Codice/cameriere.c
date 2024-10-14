  #include "header.h"

void *handle_client(void *arg) {
    Client *client = (Client *)arg;
    char buffer[BUFFER_SIZE];
    int pub_socket;
    struct sockaddr_in pub_addr;

    // Creazione del socket per il Pub
    if ((pub_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        error("Errore nella creazione del socket per il Pub");
    }

    pub_addr.sin_family = AF_INET;
    pub_addr.sin_port = htons(PUB_PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &pub_addr.sin_addr) <= 0) {
        error("Indirizzo del Pub non valido");
    }

    // Prova a connettersi al Pub e gestisce il fallimento
    if (connect(pub_socket, (struct sockaddr *)&pub_addr, sizeof(pub_addr)) < 0) {
        perror("Connessione al Pub fallita");
        close(client->socket_fd);
        close(pub_socket);
        free(client);
        return NULL;
    }

    // Riceve richiesta dal cliente
    receive_message(client->socket_fd, buffer);
    printf("Richiesta dal cliente: %s\n", buffer);

     // Invia richiesta di posto al Pub
    send_message(pub_socket, "Il cliente chiede se c'è un tavolo disponibile");
    receive_message(pub_socket, buffer);
    printf("Risposta dal Pub: %s\n", buffer);

    // Inoltra risposta al cliente
    send_message(client->socket_fd, buffer);

    if (strstr(buffer, "Posto disponibile") != NULL) {
        
        // Ricezione dell'ordine dal cliente
        receive_message(client->socket_fd, buffer);
        printf("Il cliente ha ordinato: %s\n", buffer);

        // Invia ordine al Pub
        send_message(pub_socket, buffer);
        receive_message(pub_socket, buffer);
        printf("Ordine confermato dal Pub: %s\n", buffer);

        // Invia conferma al cliente
        send_message(client->socket_fd, buffer);

        // Ricezione conferma dell'ordine pronto dal Pub
        receive_message(pub_socket, buffer);
        printf("Ordine pronto per la consegna: %s\n", buffer);
        send_message(client->socket_fd, buffer);

        // Simula pagamento
        receive_message(client->socket_fd, buffer);
        printf("Pagamento dal cliente: %s\n", buffer);
        send_message(client->socket_fd, "Grazie per aver pagato, arrivederci!");
    }

    close(pub_socket);
    close(client->socket_fd);
    free(client);
    return NULL;
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // Creazione del socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        error("Errore nella creazione del socket");
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(CAMERIERE_PORT);

    // Binding
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        error("Errore nel binding");
    }

    // Listen
    if (listen(server_fd, 3) < 0) {
        error("Errore nella listen");
    }

    printf("Cameriere avviato e in attesa di clienti...\n");

    while (1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
            error("Errore nell'accept");
        }

        Client *client = malloc(sizeof(Client));
        client->socket_fd = new_socket;
        client->address = address;

        pthread_t tid;
        pthread_create(&tid, NULL, handle_client, (void *)client);
        pthread_detach(tid);
    }

    close(server_fd);
    return 0;
}
