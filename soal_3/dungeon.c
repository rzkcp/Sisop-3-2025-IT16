#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>
#include <ctype.h>
#include <signal.h>
#include "shop_2.c"

#define PORT 8080
#define MAX_CLIENTS 10
#define MAX_INVENTORY 10
#define BUFFER_SIZE 2048  // Increased to handle larger responses

int server_fd; // Global server socket for signal handling

typedef struct {
    int money;
    char weapon[1000];
    int base_damage;
    int enemies_defeated;
    char inventory[MAX_INVENTORY][10];
    int inventory_count;
    char passive[1000];
    int active;
    int sockfd;
    int unique_id;
} Player;

Player players[MAX_CLIENTS];
int client_count = 0;
int next_player_id = 0;

// Signal handler to close all client sockets and exit cleanly
void signal_handler(int sig) {
    printf("\nServer shutting down...\n");
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (players[i].active) {
            close(players[i].sockfd);
            players[i].active = 0;
        }
    }
    close(server_fd);
    exit(0);
}

void to_lowercase(char *str) {
    for (int i = 0; str[i]; i++) {
        str[i] = tolower(str[i]);
    }
}

void send_response(int sockfd, const char *response) {
    send(sockfd, response, strlen(response), 0);
}

void clear_player_data(Player *p) {
    p->money = 0;
    strcpy(p->weapon, "");
    p->base_damage = 0;
    p->enemies_defeated = 0;
    for (int i = 0; i < p->inventory_count; i++) {
        strcpy(p->inventory[i], "");
    }
    p->inventory_count = 0;
    strcpy(p->passive, "");
    p->active = 0;
    p->sockfd = -1;
    p->unique_id = -1;
}

void update_base_damage(Player *p) {
    if (strlen(p->weapon) == 0) {
        p->base_damage = 5;
        strcpy(p->weapon, "Tangan Kosong");
    } else {
        for (int j = 0; j < shop_size; j++) {
            if (strcmp(p->weapon, shop_items[j].name) == 0) {
                p->base_damage = shop_items[j].damage;
                break;
            }
        }
    }
}

void update_passive(Player *p) {
    if (strlen(p->weapon) == 0) {
        strcpy(p->passive, "None");
    } else {
        for (int j = 0; j < shop_size; j++) {
            if (strcmp(p->weapon, shop_items[j].name) == 0) {
                strcpy(p->passive, shop_items[j].passive);
                break;
            }
        }
    }
    if (strlen(p->passive) == 0) {
        strcpy(p->passive, "None");
    }
}

void handle_client(int client_sock) {
    char buffer[BUFFER_SIZE];
    int player_id = -1;

    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (!players[i].active) {
            player_id = i;
            players[i].active = 1;
            players[i].sockfd = client_sock;
            players[i].money = 500;
            strcpy(players[i].weapon, "Tangan Kosong");
            players[i].base_damage = 5;
            players[i].enemies_defeated = 0;
            players[i].inventory_count = 0;
            strcpy(players[i].passive, "None");
            players[i].unique_id = next_player_id++;
            client_count++;
            char response[BUFFER_SIZE];
            snprintf(response, BUFFER_SIZE, "Terdaftar dengan ID: %d\n", players[i].unique_id);
            send_response(client_sock, response);
            break;
        }
    }

    if (player_id == -1) {
        send_response(client_sock, "Server penuh\n");
        close(client_sock);
        return;
    }

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        int valread = read(client_sock, buffer, BUFFER_SIZE);
        if (valread <= 0) {
            printf("Client dengan ID %d telah disconnect\n", players[player_id].unique_id);
            clear_player_data(&players[player_id]);
            client_count--;
            close(client_sock);
            return;
        }

        int choice;
        if (sscanf(buffer, "%d", &choice) != 1) {
            send_response(client_sock, "Input tidak valid, harap masukkan angka antara 1 dan 5\n");
            continue;
        }

        if (choice < 1 || choice > 5) {
            send_response(client_sock, "Pilihan menu tidak valid, harap pilih antara 1 dan 5\n");
            continue;
        }

        char response[BUFFER_SIZE] = {0};
        Player *p = &players[player_id];

        switch (choice) {
            case 1: // Show Player Stats
                snprintf(response, BUFFER_SIZE, "ID Pemain: %d\nUang: %d\nSenjata: %s\nBase Damage: %d\nMusuh Dikalahkan: %d\nPassive: %s\n",
                         p->unique_id, p->money, p->weapon, p->base_damage, p->enemies_defeated, p->passive);
                break;
            case 2: // View Inventory
                strcpy(response, "Inventori:\n");
                for (int i = 0; i < p->inventory_count; i++) {
                    char temp[100];
                    if (strcmp(p->inventory[i], p->weapon) == 0) {
                        snprintf(temp, sizeof(temp), "%d. %s (Sedang Digunakan)\n", i + 1, p->inventory[i]);
                    } else {
                        snprintf(temp, sizeof(temp), "%d. %s\n", i + 1, p->inventory[i]);
                    }
                    strcat(response, temp);
                }
                if (p->inventory_count == 0) {
                    strcat(response, "Inventori kosong\n");
                }
                break;
            case 3: // Shop (with Buying Functionality)
            {
                int total_len = 0;
                total_len += snprintf(response + total_len, BUFFER_SIZE - total_len, "Toko Senjata:\n");
                for (int i = 0; i < shop_size; i++) {
                    char temp[100];
                    int len = snprintf(temp, sizeof(temp), "%d. %s - Harga: %d, Damage: %d, Passive: %s\n",
                                       i + 1, shop_items[i].name, shop_items[i].price, shop_items[i].damage, shop_items[i].passive);
                    if (total_len + len < BUFFER_SIZE) {
                        strcpy(response + total_len, temp);
                        total_len += len;
                    } else {
                        break; // Prevent buffer overflow
                    }
                }
                total_len += snprintf(response + total_len, BUFFER_SIZE - total_len, "Pilih senjata untuk dibeli (1-6, 0 untuk kembali): ");
                send_response(client_sock, response);

                memset(buffer, 0, BUFFER_SIZE);
                read(client_sock, buffer, BUFFER_SIZE);
                int weapon_id;
                if (sscanf(buffer, "%d", &weapon_id) != 1) {
                    snprintf(response, BUFFER_SIZE, "Input tidak valid, harap masukkan angka\n");
                    send_response(client_sock, response);
                    break;
                }

                weapon_id--;

                if (weapon_id == -1) {
                    snprintf(response, BUFFER_SIZE, "Kembali ke menu utama\n");
                    send_response(client_sock, response);
                    break;
                }

                if (weapon_id < 0 || weapon_id >= shop_size) {
                    snprintf(response, BUFFER_SIZE, "Senjata tidak valid, harap pilih antara 1 dan %d\n", shop_size);
                    send_response(client_sock, response);
                    break;
                }

                printf("Debug: weapon_id = %d, shop_size = %d\n", weapon_id, shop_size); // Debug log
                if (p->money >= shop_items[weapon_id].price) {
                    p->money -= shop_items[weapon_id].price;
                    strcpy(p->inventory[p->inventory_count++], shop_items[weapon_id].name);
                    if (p->inventory_count > MAX_INVENTORY) {
                        p->inventory_count = MAX_INVENTORY; // Prevent overflow
                    }

                    if (strcmp(p->weapon, "Tangan Kosong") == 0) {
                        strcpy(p->weapon, shop_items[weapon_id].name);
                        update_base_damage(p);
                        update_passive(p);
                    }

                    snprintf(response, BUFFER_SIZE, "Berhasil membeli %s\n", shop_items[weapon_id].name);
                } else {
                    snprintf(response, BUFFER_SIZE, "Uang tidak cukup, Anda membutuhkan %d gold\n", shop_items[weapon_id].price);
                }
                send_response(client_sock, response);
                break;
            }
            case 4: // Battle Mode
            {
                static int enemy_hp = 0, max_hp = 0;
                if (enemy_hp == 0) {
                    enemy_hp = 1 + rand() % 200;
                    max_hp = enemy_hp;
                    snprintf(response, BUFFER_SIZE, "==== BATTLE STARTED ====\nEnemy appeared with:\n[#####] %d/%d HP\nType 'attack' to attack or 'exit' to leave battle.\n",
                             enemy_hp, max_hp);
                    send_response(client_sock, response);
                }

                while (enemy_hp > 0) {
                    memset(buffer, 0, BUFFER_SIZE);
                    read(client_sock, buffer, BUFFER_SIZE);
                    buffer[strcspn(buffer, "\n")] = 0;

                    to_lowercase(buffer);

                    if (strcmp(buffer, "exit") == 0) {
                        enemy_hp = 0;
                        snprintf(response, BUFFER_SIZE, "Keluar dari pertempuran\n");
                        send_response(client_sock, response);
                        break;
                    } else if (strcmp(buffer, "attack") == 0) {
                        int total_damage = p->base_damage + (rand() % 5);
                        char damage_msg[1000] = "";
                        char passive_msg[1000] = "";
                        int attack_count = 1;
                        int crit_chance = 10;
                        int is_critical = (rand() % 100) < crit_chance;
                        int is_insta_kill = 0;

                        if (strncmp(p->passive, "Summon developer", 30) == 0) {
                            total_damage *= 2;
                            strcat(damage_msg, "Developer's Wrath! ");
                            strcat(passive_msg, "Summon developer,waspada!!! konon katanya developer sedang stres berat!\n");
                        }

                        if (is_critical) {
                            total_damage *= 2;
                            strcat(damage_msg, "Critical Hit! ");
                        }

                        char passive_copy[1000];
                        strcpy(passive_copy, p->passive);
                        char *token = strtok(passive_copy, ",");
                        while (token != NULL) {
                            while (*token == ' ') token++;
                            if (strncmp(token, "Critical Chance +30%", 19) == 0) {
                                if (!is_critical) {
                                    crit_chance += 30;
                                    is_critical = (rand() % 100) < 30;
                                    if (is_critical) {
                                        total_damage *= 2;
                                        strcat(damage_msg, "Passive Critical Chance! ");
                                        strcat(passive_msg, "Passive Critical Chance +30% activated!\n");
                                    }
                                }
                            } else if (strncmp(token, "Magic Damage +5", 15) == 0) {
                                if (rand() % 100 < 50) {
                                    total_damage += 5;
                                    strcat(damage_msg, "Magic Damage +5! ");
                                    strcat(passive_msg, "Passive Magic Damage +5 activated!\n");
                                }
                            } else if (strncmp(token, "10% Insta-Kill chance", 21) == 0) {
                                if (rand() % 100 < 10) {
                                    is_insta_kill = 1;
                                    strcat(damage_msg, "Insta-Kill! ");
                                    strcat(passive_msg, "Passive 10% Insta-Kill chance activated!\n");
                                }
                            }
                            token = strtok(NULL, ",");
                        }

                        if (is_insta_kill) {
                            enemy_hp = 0;
                        } else {
                            int damage_per_attack = total_damage / attack_count;
                            for (int i = 0; i < attack_count; i++) {
                                enemy_hp -= damage_per_attack;
                            }
                        }

                        if (enemy_hp <= 0) {
                            int reward = 1 + rand() % 200;
                            p->money += reward;
                            p->enemies_defeated++;
                            snprintf(response, BUFFER_SIZE, "You dealt %d damage! %s\n%sEnemy defeated!\n==== REWARD ====\nYou earned %d gold!\n",
                                     total_damage, damage_msg, passive_msg, reward);
                            send_response(client_sock, response);

                            enemy_hp = 50 + rand() % 200;
                            max_hp = enemy_hp;
                            snprintf(response, BUFFER_SIZE, "==== NEW ENEMY ====\nEnemy health: [#####] %d/%d HP\nType 'attack' to attack or 'exit' to leave battle.\n",
                                     enemy_hp, max_hp);
                        } else {
                            char health_bar[7] = "[     ]";
                            int bars = (enemy_hp * 5) / max_hp;
                            for (int i = 1; i <= bars; i++) {
                                health_bar[i] = '#';
                            }
                            snprintf(response, BUFFER_SIZE, "You dealt %d damage! %s\n%s==== ENEMY STATUS ====\nEnemy health: %s %d/%d HP\nType 'attack' to attack or 'exit' to leave battle.\n",
                                     total_damage, damage_msg, passive_msg, health_bar, enemy_hp, max_hp);
                        }
                        send_response(client_sock, response);
                    } else {
                        snprintf(response, BUFFER_SIZE, "Opsi tidak valid\nType 'attack' to attack or 'exit' to leave battle.\n");
                        send_response(client_sock, response);
                    }
                }
                continue;
            }
            case 5: // Exit
            {
                printf("Client dengan ID %d keluar\n", p->unique_id);
                clear_player_data(p);
                client_count--;
                close(client_sock);
                return;
            }
        }
        send_response(client_sock, response);
    }
}

int main() {
    struct sockaddr_in server_addr, client_addr;
    int opt = 1;
    int addrlen = sizeof(client_addr);

    signal(SIGINT, signal_handler);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("Setsockopt failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        fprintf(stderr, "Bind failed: %s\n", strerror(errno));
        fprintf(stderr, "Port 8080 mungkin sudah digunakan. Periksa dengan perintah:\n");
        fprintf(stderr, "  - Linux/Mac: lsof -i :8080\n");
        fprintf(stderr, "  - Windows: netstat -a -n -o | find \"8080\"\n");
        fprintf(stderr, "Tutup aplikasi yang menggunakan port tersebut atau ganti port di kode.\n");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server berjalan di port %d\n", PORT);

    while (1) {
        int new_socket = accept(server_fd, (struct sockaddr *)&client_addr, (socklen_t*)&addrlen);
        if (new_socket < 0) {
            perror("Accept failed");
            continue;
        }

        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(client_addr.sin_addr), client_ip, INET_ADDRSTRLEN);
        int client_port = ntohs(client_addr.sin_port);
        printf("Client baru tersambung dari IP: %s, Port: %d\n", client_ip, client_port);

        if (client_count < MAX_CLIENTS) {
            if (fork() == 0) {
                close(server_fd);
                handle_client(new_socket);
                exit(0);
            }
            close(new_socket);
        } else {
            send_response(new_socket, "Server penuh\n");
            close(new_socket);
        }
    }

    return 0;
}