

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_ADDR "127.0.0.1"  // Alamat server (localhost)
#define PORT 9090                // Port yang digunakan untuk koneksi

// Fungsi untuk melakukan reverse dan decode hex
void decryptText(char *inputFile) {
    // Baca isi file input
    FILE *file = fopen(inputFile, "r");
    if (!file) {
        perror("Gagal membuka file");
        return;
    }

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *buffer = malloc(length + 1);
    fread(buffer, 1, length, file);
    buffer[length] = '\0';

    // Reverse text
    int i, j;
    char temp;
    for (i = 0, j = length - 1; i < j; i++, j--) {
        temp = buffer[i];
        buffer[i] = buffer[j];
        buffer[j] = temp;
    }

    // Print reversed text (for testing purposes)
    printf("Decrypted Text: \n%s\n", buffer);

    // Lakukan decoding Hex jika perlu di sini

    free(buffer);
    fclose(file);
}

int main() {
    int sockfd;
    struct sockaddr_in serverAddr;
    char buffer[1024];
    char inputFile[256];

    // Membuat socket untuk koneksi ke server
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Gagal membuat socket");
        exit(1);
    }

    // Setup server address
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_ADDR);

    // Menghubungkan ke server
    if (connect(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Gagal connect ke server");
        close(sockfd);
        exit(1);
    }

    printf("Terhubung ke server di %s:%d\n", SERVER_ADDR, PORT);

    // Menu untuk memilih perintah
    while (1) {
        printf("\n--- MENU ---\n");
        printf("1. DECRYPT\n");
        printf("2. DOWNLOAD\n");
        printf("3. EXIT\n");
        printf("Pilih perintah (1/2/3): ");
        fgets(buffer, sizeof(buffer), stdin);
        int choice = atoi(buffer);

        if (choice == 1) {
            // DECRYPT: Minta input nama file
            printf("Masukkan nama file input untuk dekripsi (misal: input_1.txt): ");
            fgets(inputFile, sizeof(inputFile), stdin);
            inputFile[strcspn(inputFile, "\n")] = 0;  // Menghapus newline di akhir

            // Kirim perintah DECRYPT ke server
            send(sockfd, "DECRYPT", 7, 0);
            send(sockfd, inputFile, strlen(inputFile), 0);
            decryptText(inputFile);  // Dekripsi lokal (untuk testing)
        }
        else if (choice == 2) {
            // DOWNLOAD: Minta filename dari server
            printf("Masukkan nama file yang ingin diunduh (misal: 1744401282.jpeg): ");
            fgets(buffer, sizeof(buffer), stdin);
            buffer[strcspn(buffer, "\n")] = 0;  // Menghapus newline di akhir

            // Kirim perintah DOWNLOAD ke server
            send(sockfd, "DOWNLOAD", 8, 0);
            send(sockfd, buffer, strlen(buffer), 0);

            // Terima file dari server
            FILE *file = fopen(buffer, "wb");
            if (!file) {
                perror("Gagal membuka file untuk ditulis");
                continue;
            }

            int bytesReceived = 0;
            while ((bytesReceived = recv(sockfd, buffer, sizeof(buffer), 0)) > 0) {
                fwrite(buffer, 1, bytesReceived, file);
            }

            if (bytesReceived == 0) {
                printf("File berhasil diunduh: %s\n", buffer);
            } else if (bytesReceived < 0) {
                perror("Gagal menerima file");
            }

            fclose(file);
        }
        else if (choice == 3) {
            // EXIT: Keluar dari program
            send(sockfd, "EXIT", 4, 0);
            break;
        }
        else {
            printf("Pilihan tidak valid. Coba lagi.\n");
        }
    }

    // Tutup koneksi ke server
    close(sockfd);
    printf("Koneksi ditutup.\n");

    return 0;
}
