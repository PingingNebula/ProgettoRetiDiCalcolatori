#include "header.h"

// Menu delle portate
const char *menu[] = {
    "1. Birra",
    "2. Vino",
    "3. Panino Porchetta e provola",
    "4. Patatine e carne",
    "5. Pizza Margherita",
    "6. Hot Dog",
    "7. Hamburger cheddar e patatine",
    "8. Panino Salsiccia e friarielli",
    "9. Fanta",
    "10. Gelato",
    "11. Caffè",
    "12. Limoncello",
    "13. Bustina ketchup",
    "14. Bustina maionese"
};


// Funzione per mostrare il menu
void mostra_menu() {
    printf("Menu:\n");
    for (int i = 0; i < 14; i++) {
        printf("%s\n", menu[i]);
    }
}


// Funzione per l'invio di messaggi attraverso il socket
void send_message(int socket, const char *message) {
    int length = strlen(message);
    if (send(socket, message, length, 0) != length) {
        error("Errore nell'invio del messaggio");
    }
}

// Funzione per la ricezione di messaggi attraverso il socket
int receive_message(int socket, char *buffer) {  
    int valread = recv(socket, buffer, BUFFER_SIZE, 0);
    if (valread <= 0) {
        perror("Errore nella ricezione del messaggio");
        return -1;  // Ritorna -1 in caso di errore
    }
    buffer[valread] = '\0'; // Terminazione stringa
    return valread;  // Ritorna il numero di byte letti
}

// Funzione di gestione errori
void error(const char *msg) {
    perror(msg);
    exit(1);
}
