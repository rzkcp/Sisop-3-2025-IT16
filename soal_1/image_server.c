// image_server.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <signal.h>
#include <errno.h>

#define PORT 9090
#define BUFFER_SIZE 8192
#define DB_PATH "server/database"
#define LOG_PATH "server/server.log"

void write_log(const char *source, const char *action, const char *info) {
    FILE *log = fopen(LOG_PATH, "a");
    if (!log) return;

    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char timestamp[64];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", t);

    fprintf(log, "[%s][%s]: [%s] [%s]\n", source, timestamp, action, info);
    fclose(log);
}

void daemonize() {
    pid_t pid = fork();
    if (pid < 0) exit(EXIT_FAILURE);
    if (pid > 0) exit(EXIT_SUCCESS); // Parent keluar

    // Jadi session leader
    if (setsid() < 0) exit(EXIT_FAILURE);

    // Fork kedua untuk memastikan bukan terminal leader
    pid = fork();
    if (pid < 0) exit(EXIT_FAILURE);
    if (pid > 0) exit(EXIT_SUCCESS);

    umask(0);
    chdir("/");

    // Tutup file descriptor
    for (int x = sysconf(_SC_OPEN_MAX); x >= 0; x--) close(x);
}

char *timestamp_filename() {
    time_t now = time(NULL);
    char *filename = malloc(32);
    snprintf(filename, 32, "%ld.jpeg", now);
    return filename;
}

void handle_client(int client_sock) {
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;

    while ((bytes_read = recv(client_sock, buffer, sizeof(buffer), 0)) > 0) {
        buffer[bytes_read] = '\0';

        // Handle DECRYPT
        if (strncmp(buffer, "DECRYPT:", 8) == 0) {
            write_log("Client", "DECRYPT", "Text data");

            // Simulasi simpan sebagai JPEG
            char *filename = timestamp_filename();
            char path[256];
            snprintf(path, sizeof(path), "%s/%s", DB_PATH, filename);

            FILE *f = fopen(path, "wb");
            if (f) {
                fwrite(buffer + 8, 1, bytes_read - 8, f);
                fclose(f);

                write_log("Server", "SAVE", filename);
                send(client_sock, filename, strlen(filename), 0);
            } else {
                write_log("Server", "ERROR", "Gagal simpan file");
                send(client_sock, "ERROR: Gagal simpan file", 25, 0);
            }
            free(filename);
        }

        // Handle DOWNLOAD
        else if (strncmp(buffer, "DOWNLOAD:", 9) == 0) {
            char filename[128];
            sscanf(buffer + 9, "%127s", filename);

            char path[256];
            snprintf(path, sizeof(path), "%s/%s", DB_PATH, filename);

            FILE *f = fopen(path, "rb");
            if (f) {
                write_log("Client", "DOWNLOAD", filename);
                write_log("Server", "UPLOAD", filename);

                while ((bytes_read = fread(buffer, 1, sizeof(buffer), f)) > 0)
                    send(client_sock, buffer, bytes_read, 0);
                fclose(f);
            } else {
                write_log("Server", "ERROR", "File tidak ditemukan");
                send(client_sock, "ERROR: File tidak ditemukan", 28, 0);
            }
        }

        // Exit
        else if (strncmp(buffer, "EXIT", 4) == 0) {
            write_log("Client", "EXIT", "Client requested to exit");
            break;
        }

        // Invalid command
        else {
            write_log("Server", "ERROR", "Perintah tidak dikenali");
            send(client_sock, "ERROR: Perintah tidak dikenali", 30, 0);
        }
    }

    close(client_sock);
}

int main() {
    daemonize();

    mkdir("server", 0755);
    mkdir(DB_PATH, 0755);

    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(struct sockaddr_in);

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) exit(EXIT_FAILURE);

    int opt = 1;
    setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    memset(&(server_addr.sin_zero), '\0', 8);

    bind(server_sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr));
    listen(server_sock, 10);

    while (1) {
        client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &addr_len);
        if (client_sock < 0) continue;

        if (!fork()) {
            close(server_sock);
            handle_client(client_sock);
            exit(0);
        }
        close(client_sock);
    }

    return 0;
}
