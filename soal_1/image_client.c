#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8080
#define MAX_BUFFER 8192
#define SECRETS_DIR "client/secrets/"

void send_all(int sock, const void *buf, size_t len) {
    size_t sent = 0;
    while (sent < len) {
        ssize_t n = send(sock, buf + sent, len - sent, 0);
        if (n <= 0) break;
        sent += n;
    }
}

int reconnect(int *sock) {
    close(*sock);
    if ((*sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Gagal membuat socket\n");
        return -1;
    }
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("Alamat tidak valid\n");
        return -1;
    }
    if (connect(*sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("Gagal connect ke server\n");
        return -1;
    }
    printf("Reconnected to address 127.0.0.1:8080\n");
    return 0;
}

int main() {
    int sock = 0;
    
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Gagal membuat socket\n");
        return -1;
    }
    
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("Alamat tidak valid\n");
        return -1;
    }
    
    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("Gagal connect ke server\n");
        return -1;
    }
    printf("Connected to address 127.0.0.1:8080\n");

    while (1) {
        printf("\n________________________________________________\n");
        printf("| Image Decoder Client |\n");
        printf("|______________________________________________|\n");
        printf("1. Send input file to server\n");
        printf("2. Download file from server\n");
        printf("3. Exit\n");
        printf("> ");
        
        int choice;
        scanf("%d", &choice);
        getchar();

        if (choice == 1) {
            printf("Enter the file name: ");
            char filename[50];
            fgets(filename, 50, stdin);
            filename[strcspn(filename, "\n")] = '\0';
            
            printf("File name entered: %s\n", filename);
            
            char fullpath[100];
            snprintf(fullpath, sizeof(fullpath), "%s%s", SECRETS_DIR, filename);
            FILE *file = fopen(fullpath, "r");
            if (!file) {
                printf("Salah nama text file input\n");
                continue;
            }
            
            char content[MAX_BUFFER];
            size_t len = fread(content, 1, MAX_BUFFER - 1, file);
            content[len] = '\0';
            fclose(file);
            
            char command[] = "DECRYPT|";
            send_all(sock, command, strlen(command));
            send_all(sock, content, len);
            
            char response[MAX_BUFFER] = {0};
            int valread = read(sock, response, MAX_BUFFER);
            if (valread > 0) {
                response[valread] = '\0';
                printf("Server: %s\n", response);
            } else {
                printf("Server disconnected\n");
                if (reconnect(&sock) == 0) continue;
                break;
            }
        } else if (choice == 2) {
            printf("Enter the file name: ");
            char filename[50];
            fgets(filename, 50, stdin);
            filename[strcspn(filename, "\n")] = '\0';
            
            printf("File name entered: %s\n", filename);
            
            char command[MAX_BUFFER];
            snprintf(command, sizeof(command), "DOWNLOAD|%s", filename);
            send_all(sock, command, strlen(command));
            
            char buffer[MAX_BUFFER] = {0};
            int valread = read(sock, buffer, MAX_BUFFER);
            if (valread > 0) {
                if (strncmp(buffer, "Gagal", 5) == 0 || strncmp(buffer, "Invalid", 7) == 0) {
                    buffer[valread] = '\0';
                    printf("Server: %s\n", buffer);
                } else {
                    char output_path[100];
                    snprintf(output_path, sizeof(output_path), "client/%s", filename);
                    FILE *out = fopen(output_path, "wb");
                    if (out) {
                        fwrite(buffer, 1, valread, out);
                        fclose(out);
                        printf("Success! IMAGE saved as %s\n", filename);
                    }
                }
            } else {
                printf("Server disconnected\n");
                if (reconnect(&sock) == 0) continue;
                break;
            }
        } else if (choice == 3) {
            send_all(sock, "EXIT", 4);
            close(sock);
            break;
        }
    }
    close(sock);
    return 0;
}
