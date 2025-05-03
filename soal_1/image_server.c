
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <arpa/inet.h>

#define PORT 8080
#define MAX_BUFFER 4096
#define MAX_PATH_LEN 512
#define DATABASE_DIR "server/database/"
#define LOG_FILE "server/server.log"

void log_action(const char *source, const char *action, const char *info) {
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    char timestamp[20];
    strftime(timestamp, 20, "%Y-%m-%d %H:%M:%S", tm_info);
    
    FILE *log = fopen(LOG_FILE, "a");
    if (log) {
        fprintf(log, "[%s][%s]: [%s] [%s]\n", source, timestamp, action, info);
        fclose(log);
    }
}

void daemonize() {
    pid_t pid = fork();
    if (pid < 0) exit(EXIT_FAILURE);
    if (pid > 0) exit(EXIT_SUCCESS);
    
    setsid();
    chdir("/");
    umask(0);
    
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
}

char* reverse_string(const char *str) {
    size_t len = strlen(str);
    char *reversed = malloc(len + 1);
    for (size_t i = 0; i < len; i++) {
        reversed[i] = str[len - 1 - i];
    }
    reversed[len] = '\0';
    return reversed;
}

unsigned char* hex_decode(const char *hex, size_t *out_len) {
    size_t len = strlen(hex);
    if (len % 2 != 0) return NULL;
    *out_len = len / 2;
    unsigned char *bin = malloc(*out_len);
    
    for (size_t i = 0; i < *out_len; i++) {
        sscanf(hex + 2*i, "%2hhx", &bin[i]);
    }
    return bin;
}

void handle_client(int client_sock) {
    char cmd;
    
    while (1) {
        if (recv(client_sock, &cmd, 1, 0) <= 0) break;
        
        if (cmd == 'D') {
            uint32_t data_len;
            recv(client_sock, &data_len, 4, 0);
            data_len = ntohl(data_len);
            
            char *text = malloc(data_len + 1);
            ssize_t received = recv(client_sock, text, data_len, 0);
            if (received != data_len) {
                free(text);
                continue;
            }
            text[data_len] = '\0';
            
            char *reversed = reverse_string(text);
            size_t bin_len;
            unsigned char *bin = hex_decode(reversed, &bin_len);
            
            if (bin) {
                time_t timestamp = time(NULL);
                char filename[MAX_PATH_LEN];
                snprintf(filename, sizeof(filename), "%ld.jpeg", timestamp);
                
                char fullpath[MAX_PATH_LEN];
                int path_len = snprintf(fullpath, sizeof(fullpath), "%s%s", DATABASE_DIR, filename);
                
                if (path_len > 0 && (size_t)path_len < sizeof(fullpath)) {
                    FILE *file = fopen(fullpath, "wb");
                    if (file) {
                        fwrite(bin, 1, bin_len, file);
                        fclose(file);
                        send(client_sock, "S", 1, 0);
                        log_action("Client", "DECRYPT", "Text data");
                        log_action("Server", "SAVE", filename);
                    }
                }
                free(bin);
            }
            free(reversed);
            free(text);
            
        } else if (cmd == 'L') {
            uint32_t name_len;
            recv(client_sock, &name_len, 4, 0);
            name_len = ntohl(name_len);
            
            if (name_len >= MAX_BUFFER) {
                send(client_sock, "E", 1, 0);
                log_action("Server", "ERROR", "Filename too long");
                continue;
            }
            
            char filename[MAX_BUFFER];
            ssize_t received = recv(client_sock, filename, name_len, 0);
            if (received != name_len) {
                send(client_sock, "E", 1, 0);
                continue;
            }
            filename[name_len] = '\0';
            
            char fullpath[MAX_PATH_LEN];
            int path_len = snprintf(fullpath, sizeof(fullpath), "%s%s", DATABASE_DIR, filename);
            
            if (path_len < 0 || (size_t)path_len >= sizeof(fullpath)) {
                send(client_sock, "E", 1, 0);
                log_action("Server", "ERROR", "Path truncated");
                continue;
            }
            
            FILE *file = fopen(fullpath, "rb");
            if (file) {
                fseek(file, 0, SEEK_END);
                long file_size = ftell(file);
                fseek(file, 0, SEEK_SET);
                
                unsigned char *file_data = malloc(file_size);
                fread(file_data, 1, file_size, file);
                fclose(file);
                
                send(client_sock, "S", 1, 0);
                uint32_t net_size = htonl(file_size);
                send(client_sock, &net_size, 4, 0);
                send(client_sock, file_data, file_size, 0);
                
                log_action("Client", "DOWNLOAD", filename);
                log_action("Server", "UPLOAD", filename);
                free(file_data);
            } else {
                send(client_sock, "E", 1, 0);
            }
        } else if (cmd == 'X') {
            break;
        }
    }
    close(client_sock);
}

int main() {
    daemonize();
    mkdir(DATABASE_DIR, 0777);
    
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    
    bind(server_fd, (struct sockaddr*)&address, sizeof(address));
    listen(server_fd, 5);
    
    while (1) {
        int client_sock = accept(server_fd, NULL, NULL);
        if (client_sock < 0) continue;
        handle_client(client_sock);
    }
    
    return 0;
}
