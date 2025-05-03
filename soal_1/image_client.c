
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8080
#define MAX_BUFFER 4096
#define SECRETS_DIR "client/secrets/"

void send_all(int sock, const void *buf, size_t len) {
    size_t sent = 0;
    while (sent < len) {
        ssize_t n = send(sock, buf + sent, len - sent, 0);
        if (n <= 0) break;
        sent += n;
    }
}

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Gagal membuat socket\n");
        return -1;
    }
    
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
    
    while (1) {
        printf("\nMenu:\n");
        printf("1. Decrypt file\n");
        printf("2. Download file\n");
        printf("3. Exit\n");
        printf("Pilih: ");
        
        int choice;
        scanf("%d", &choice);
        getchar();

        if (choice == 1) {
            printf("Masukkan nama file (input_1.txt s/d input_5.txt): ");
            char filename[50];
            fgets(filename, 50, stdin);
            filename[strcspn(filename, "\n")] = '\0';
            
            char fullpath[100];
            snprintf(fullpath, sizeof(fullpath), "%s%s", SECRETS_DIR, filename);
            printf("DEBUG: Mencari file di %s\n", fullpath); // Debug path
            
            FILE *file = fopen(fullpath, "r");
            if (!file) {
                printf("File tidak ditemukan\n");
                continue;
            }
            
            // ... (sisa kode decrypt)
            
        } else if (choice == 2) {
            // ... (kode download)
        } else if (choice == 3) {
            break;
        }
    }
    close(sock);
    return 0;
}
