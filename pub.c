#include "header.h"

// Variabili globali
Pub pub = {MAX_POSTI, {0}, PTHREAD_MUTEX_INITIALIZER}; 

// Funzione per inizializzare i tavoli del Pub
void inizializza_tavoli_pub() {
    for (int i = 0; i < MAX_POSTI; i++) {
        pub.posti[i] = i + 1;  
    }
}

// Funzione per assegnare un tavolo disponibile
int assegna_posto() {
    for (int i = 0; i < MAX_POSTI; i++) {
        if (pub.posti[i] != 0) {  
            int num_posto = pub.posti[i];
            pub.posti[i] = 0;     
            pub.posti_liberi--;
            return num_posto;
        }
    }
    return -1; // Nessun posto disponibile
}

// Funzione per liberare un tavolo
void libera_posto(int num_posto) {
    for (int i = 0; i < MAX_POSTI; i++) {
        if (pub.posti[i] == 0) { 
            pub.posti[i] = num_posto; 
            pub.posti_liberi++;
            break;
        }
    }
}

// Funzione per gestire il cliente connesso
void *handle_client(void *arg) {
    Client *client = (Client *)arg;
    char buffer[BUFFER_SIZE];
    int num_posto = -1;

    // Verifica posti disponibili e assegna un tavolo
    pthread_mutex_lock(&pub.mutex);
    num_posto = assegna_posto();
    pthread_mutex_unlock(&pub.mutex);

    if (num_posto != -1) {

        // Invia al cameriere il numero del tavolo assegnato
        sprintf(buffer, "Posto disponibile, tavolo libero numero %d\n", num_posto);
        send_message(client->socket_fd, buffer);
    } else {

        // Invia un messaggio che indica che non ci sono posti disponibili
        send_message(client->socket_fd, "Spiacente, nessun posto disponibile.");
        close(client->socket_fd);
        free(client);
        return NULL;
    }

    // Ricezione del primo messaggio dal cameriere
    printf("In attesa del cameriere...\n");
    if (receive_message(client->socket_fd, buffer) < 0) {
        perror("Errore nella ricezione del primo messaggio dal cameriere\n");
        close(client->socket_fd);
        free(client);
        return NULL;
    }
    printf("Il cameriere domanda.: %s\n", buffer);

    // Invia una conferma di ricezione al cameriere
    send_message(client->socket_fd, "Ordine pronto per il ritiro dal cameriere");

    // Ricezione dell'ordine dal cameriere
    printf("In attesa del cameriere che ritira l'ordine...\n");
    if (receive_message(client->socket_fd, buffer) < 0) {
        perror("Errore nella ricezione dell'ordine");
        close(client->socket_fd);
        free(client);
        return NULL;
    }

 
        // Stampa l'ordine ricevuto con il nome del prodotto
        printf("Ordine ricevuto dal cameriere per il tavolo %d: %s\n", num_posto, buffer);

        // Simulazione della preparazione dell'ordine
        send_message(client->socket_fd, "Ordine confermato. Preparazione in corso...");
        sleep(1); 
        send_message(client->socket_fd, "Ordine pronto. Consegna in corso...");

    close(client->socket_fd);

    // Libera il tavolo dopo che il cliente ha finito
    pthread_mutex_lock(&pub.mutex);
    libera_posto(num_posto);
    pthread_mutex_unlock(&pub.mutex);

    free(client);
    return NULL;
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // Inizializza i tavoli del Pub
    inizializza_tavoli_pub();

    // Creazione del socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        error("Errore nella creazione del socket");
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PUB_PORT);

    // Binding del socket
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        error("Errore nel binding");
    }

    // Imposta il server in ascolto
    if (listen(server_fd, 3) < 0) {
        error("Errore nella listen");
    }

    printf("Pub aperto e in attesa di clienti...\n");

    // Ciclo principale per accettare connessioni dai clienti
    while (1) {
        // Accetta nuove connessioni in arrivo
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
            perror("Errore nell'accept");
            continue;
        }

        // Creazione del client e gestione del thread
        Client *client = malloc(sizeof(Client));
        if (client == NULL) {
            perror("Errore nell'allocazione della memoria per il client");
            close(new_socket);
            continue;
        }

        client->socket_fd = new_socket;
        client->address = address;

        // Creazione di un nuovo thread per gestire il client
        pthread_t tid;
        if (pthread_create(&tid, NULL, handle_client, (void *)client) != 0) {
            perror("Errore nella creazione del thread");
            close(new_socket);
            free(client);
            continue;
        }

        pthread_detach(tid); // Assicura che le risorse del thread siano liberate al termine
    }

    close(server_fd);
    return 0;
}
