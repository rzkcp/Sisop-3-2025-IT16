#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/stat.h>
#include <syslog.h>

#define PORT 8080
#define MAX_BUFFER 8192
#define DATABASE_DIR "server/database/"

void daemonize() {
    pid_t pid = fork();
    if (pid < 0) exit(EXIT_FAILURE);
    if (pid > 0) exit(EXIT_SUCCESS);
    umask(0);
    setsid();
    chdir("/");
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
    open("/dev/null", O_RDONLY);
    open("/dev/null", O_WRONLY);
    open("/dev/null", O_WRONLY);
}

void log_action(const char *source, const char *action, const char *info) {
    mkdir("server", 0755);
    FILE *log = fopen("server/server.log", "a");
    if (log) {
        time_t now = time(NULL);
        struct tm *tm = localtime(&now);
        fprintf(log, "[%s][%04d-%02d-%02d %02d:%02d:%02d]: [%s] [%s]\n",
                source, tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
                tm->tm_hour, tm->tm_min, tm->tm_sec, action, info);
        fflush(log);
        fclose(log);
    }
}

void send_all(int sock, const void *buf, size_t len) {
    size_t sent = 0;
    while (sent < len) {
        ssize_t n = send(sock, buf + sent, len - sent, 0);
        if (n <= 0) {
            log_action("Server", "ERROR", "Send failed");
            break;
        }
        sent += n;
    }
}

void reverse_and_hex_decode(const char *data, int len, char *output_filename) {
    char reversed[len + 1];
    for (int i = 0; i < len; i++) reversed[i] = data[len - 1 - i];
    reversed[len] = '\0';

    mkdir("server", 0755);
    mkdir("server/database", 0755);
    FILE *out = fopen(output_filename, "wb");
    if (out) {
        for (int i = 0; i < len; i += 2) {
            char hex[3] = {reversed[i], reversed[i + 1], '\0'};
            unsigned char byte = (unsigned char)strtol(hex, NULL, 16);
            fwrite(&byte, 1, 1, out);
        }
        fclose(out);
        if (access(output_filename, F_OK) == 0) {
            log_action("Server", "INFO", "File created successfully");
        } else {
            log_action("Server", "ERROR", "Failed to create file");
        }
    } else {
        log_action("Server", "ERROR", "Failed to open file for writing");
    }
}

int main() {
    daemonize();
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[MAX_BUFFER] = {0};

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        log_action("Server", "ERROR", "Gagal membuat socket");
        return -1;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        log_action("Server", "ERROR", "Gagal bind socket");
        return -1;
    }

    if (listen(server_fd, 10) < 0) {
        log_action("Server", "ERROR", "Gagal listen");
        return -1;
    }

    while (1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            log_action("Server", "ERROR", "Gagal accept connection");
            continue;
        }

        while (1) {
            int valread = read(new_socket, buffer, MAX_BUFFER);
            if (valread <= 0) {
                log_action("Server", "INFO", "Client disconnected");
                break;
            }

            buffer[valread] = '\0';
            char *command = strtok(buffer, "|");
            char *data = strtok(NULL, "");

            if (command && strcmp(command, "DECRYPT") == 0 && data) {
                char log_msg[MAX_BUFFER];
                snprintf(log_msg, sizeof(log_msg), "Text data");
                log_action("Client", "DECRYPT", log_msg);

                char timestamp[20];
                snprintf(timestamp, sizeof(timestamp), "%ld.jpeg", time(NULL));
                char output_path[100];
                snprintf(output_path, sizeof(output_path), "%s%s", DATABASE_DIR, timestamp);

                reverse_and_hex_decode(data, strlen(data), output_path);
                log_action("Server", "SAVE", timestamp);
                char response[100];
                snprintf(response, sizeof(response), "Text decrypted and saved as %s", timestamp);
                send_all(new_socket, response, strlen(response));
            } else if (command && strcmp(command, "DOWNLOAD") == 0 && data) {
                char log_msg[MAX_BUFFER];
                snprintf(log_msg, sizeof(log_msg), "%s", data);
                log_action("Client", "DOWNLOAD", log_msg);

                char fullpath[100];
                snprintf(fullpath, sizeof(fullpath), "%s%s", DATABASE_DIR, data);
                FILE *file = fopen(fullpath, "rb");
                if (file) {
                    log_action("Server", "INFO", "File opened successfully");
                    fseek(file, 0, SEEK_END);
                    long size = ftell(file);
                    fseek(file, 0, SEEK_SET);

                    // Kirim status SUCCESS dan ukuran file
                    char status[10] = "SUCCESS|";
                    send_all(new_socket, status, strlen(status));
                    char size_str[32];
                    snprintf(size_str, sizeof(size_str), "%ld|", size);
                    send_all(new_socket, size_str, strlen(size_str));

                    // Kirim file secara bertahap
                    char chunk[MAX_BUFFER];
                    size_t bytes_left = size;
                    while (bytes_left > 0) {
                        size_t to_read = bytes_left > MAX_BUFFER ? MAX_BUFFER : bytes_left;
                        size_t bytes_read = fread(chunk, 1, to_read, file);
                        if (bytes_read <= 0) {
                            log_action("Server", "ERROR", "Failed to read file chunk");
                            break;
                        }
                        send_all(new_socket, chunk, bytes_read);
                        bytes_left -= bytes_read;
                    }
                    log_action("Server", "UPLOAD", data);
                    fclose(file);
                } else {
                    log_action("Server", "ERROR", "Gagal menemukan file untuk dikirim");
                    char error_msg[] = "ERROR|Gagal menemukan file untuk dikirim|";
                    send_all(new_socket, error_msg, strlen(error_msg));
                }
            } else if (command && strcmp(command, "EXIT") == 0) {
                log_action("Client", "EXIT", "Client requested to exit");
                break;
            } else {
                log_action("Server", "ERROR", "Invalid command received");
                send_all(new_socket, "ERROR|Invalid command|", 22);
            }
        }
        close(new_socket);
    }
    close(server_fd);
    return 0;
}
