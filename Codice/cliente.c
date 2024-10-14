  #include "header.h"

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        error("Errore nella creazione del socket");
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(CAMERIERE_PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        error("Indirizzo non valido o non supportato");
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        error("Connessione fallita");
    }

    // Chiede se ci sono posti disponibili
send_message(sock, "Salve, è disponibile un tavolo?");
receive_message(sock, buffer);
printf("Risposta dal cameriere: %s\n", buffer);

// Se c'è un tavolo disponibile, estrai il numero del tavolo dalla risposta
int num_posto = -1;
if (sscanf(buffer, "Posto disponibile, tavolo numero %d", &num_posto) == 1) {
    printf("Ti è stato assegnato il tavolo numero %d.\n", num_posto);
}
    if (strstr(buffer, "Posto disponibile") != NULL) {
        // Mostra il menu
        mostra_menu();

        // L'utente sceglie dal menu
        printf("Inserisci il numero degli elementi che desideri ordinare (es. 1 3 5): ");
        fgets(buffer, BUFFER_SIZE, stdin);

        // Invia l'ordine al cameriere
        send_message(sock, buffer);

        // Riceve conferma dal cameriere
        receive_message(sock, buffer);
        printf("Conferma dal cameriere: %s\n", buffer);

        // Riceve l'ordine preparato
        receive_message(sock, buffer);
        printf("Hai ricevuto il tuo ordine. Buon appetito!\n");

        // Simula pagamento
        send_message(sock, "Pagamento effettuato");
        receive_message(sock, buffer);
        printf("Hai ricevuto il conto: %s\n", buffer);
    }

    close(sock);
    return 0;
}
