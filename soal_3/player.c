#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"
#define PORT 8080
#define BUFFER_SIZE 2048  // Increased to match server

void display_menu() {
    printf("\nMenu Utama:\n");
    printf("1. Show Player Stats\n");
    printf("2. View Inventory\n");
    printf("3. Shop\n");
    printf("4. Battle Mode\n");
    printf("5. Exit\n");
    printf("Pilih: ");
}

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\nSocket creation error\n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported\n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed\n");
        return -1;
    }

    int valread = read(sock, buffer, BUFFER_SIZE);
    if (valread <= 0) {
        printf("Server disconnected\n");
        close(sock);
        return 0;
    }
    printf("%s", buffer);

    while (1) {
        display_menu();
        int choice;
        scanf("%d", &choice);
        getchar(); // Clear newline

        char choice_str[10];
        snprintf(choice_str, sizeof(choice_str), "%d\n", choice);
        send(sock, choice_str, strlen(choice_str), 0);

        memset(buffer, 0, BUFFER_SIZE);
        valread = read(sock, buffer, BUFFER_SIZE);
        if (valread <= 0) {
            printf("Server disconnected\n");
            close(sock);
            return 0;
        }
        printf("%s", buffer);

        if (choice == 3) { // Shop
            int weapon_choice;
            scanf("%d", &weapon_choice);
            getchar(); // Clear newline
            char weapon_choice_str[10];
            snprintf(weapon_choice_str, sizeof(weapon_choice_str), "%d\n", weapon_choice);
            send(sock, weapon_choice_str, strlen(weapon_choice_str), 0);

            memset(buffer, 0, BUFFER_SIZE);
            valread = read(sock, buffer, BUFFER_SIZE);
            if (valread <= 0) {
                printf("Server disconnected\n");
                close(sock);
                return 0;
            }
            printf("%s", buffer);
        } else if (choice == 4) { // Battle Mode
            while (1) {
                char command[10];
                scanf("%s", command);
                getchar(); // Clear newline
                strcat(command, "\n");
                send(sock, command, strlen(command), 0);

                memset(buffer, 0, BUFFER_SIZE);
                valread = read(sock, buffer, BUFFER_SIZE);
                if (valread <= 0) {
                    printf("Server disconnected\n");
                    close(sock);
                    return 0;
                }
                printf("%s", buffer);

                if (strstr(buffer, "Keluar dari pertempuran") != NULL) {
                    break;
                }
            }
        } else if (choice == 5) { // Exit
            printf("Keluar dari permainan\n");
            close(sock);
            return 0;
        }
    }

    close(sock);
    return 0;
}