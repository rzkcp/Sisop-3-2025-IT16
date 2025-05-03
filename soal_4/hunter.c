#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>
#include <unistd.h>

#define SHM_HUNTER_KEY 1234
#define SHM_DUNGEON_KEY 5678
#define MAX_HUNTERS 100
#define MAX_DUNGEONS 50

typedef struct {
    char password[50];
    char name[50];
    int level;
    int exp;
    int atk;
    int hp;
    int def;
    int banned;
} Hunter;

typedef struct {
    char name[50];
    int min_level;
    int reward_atk;
    int reward_hp;
    int reward_def;
    int reward_exp;
    char key[50];
} Dungeon;

typedef struct {
    Hunter hunters[MAX_HUNTERS];
    int hunter_count;
} HunterShared;

typedef struct {
    Dungeon dungeons[MAX_DUNGEONS];
    int dungeon_count;
} DungeonShared;

int login(HunterShared *hshm, char *name, char *password, Hunter **hunter) {
    for (int i = 0; i < hshm->hunter_count; i++) {
        if (strcmp(hshm->hunters[i].name, name) == 0) {
            if (strcmp(hshm->hunters[i].password, password) == 0) {
                *hunter = &hshm->hunters[i];
                return 1;
            } else {
                printf("Password salah!\n");
                return 0;
            }
        }
    }
    printf("Nama hunter tidak ditemukan!\n");
    return 0;
}

void register_hunter(HunterShared *hshm, char *name, char *password) {
    if (hshm->hunter_count >= MAX_HUNTERS) {
        printf("Kapasitas hunter penuh!\n");
        return;
    }
    for (int i = 0; i < hshm->hunter_count; i++) {
        if (strcmp(hshm->hunters[i].name, name) == 0) {
            printf("Nama sudah digunakan, pilih nama lain!\n");
            return;
        }
    }
    Hunter *h = &hshm->hunters[hshm->hunter_count];
    strcpy(h->name, name);
    strcpy(h->password, password);
    h->level = 1;
    h->exp = 0;
    h->atk = 10;
    h->hp = 100;
    h->def = 5;
    h->banned = 0;
    hshm->hunter_count++;
    printf("Hunter %s terdaftar!\n", name);
}

void show_dungeons_for_level(DungeonShared *dshm, int level) {
    printf("Dungeon tersedia untuk level %d:\n", level);
    for (int i = 0; i < dshm->dungeon_count; i++) {
        if (dshm->dungeons[i].min_level <= level) {
            Dungeon *d = &dshm->dungeons[i];
            printf("Key: %s, Nama: %s, Min Level: %d, Reward: ATK=%d, HP=%d, DEF=%d, EXP=%d\n",
                   d->key, d->name, d->min_level, d->reward_atk, d->reward_hp, d->reward_def, d->reward_exp);
        }
    }
}

void conquer_dungeon(Hunter *hunter, DungeonShared *dshm, char *dungeon_key) {
    if (hunter->banned) {
        printf("Kamu dibanned dari raid!\n");
        return;
    }
    for (int i = 0; i < dshm->dungeon_count; i++) {
        if (strcmp(dshm->dungeons[i].key, dungeon_key) == 0) {
            Dungeon *d = &dshm->dungeons[i];
            if (d->min_level > hunter->level) {
                printf("Level hunter tidak cukup!\n");
                return;
            }
            hunter->atk += d->reward_atk;
            hunter->hp += d->reward_hp;
            hunter->def += d->reward_def;
            hunter->exp += d->reward_exp;
            printf("Dungeon %s ditaklukkan! Mendapat: ATK=%d, HP=%d, DEF=%d, EXP=%d\n",
                   d->name, d->reward_atk, d->reward_hp, d->reward_def, d->reward_exp);
            if (hunter->exp >= 500) {
                hunter->level++;
                hunter->exp = 0;
                printf("Level up! Sekarang level %d\n", hunter->level);
            }
            // Hapus dungeon
            for (int j = i; j < dshm->dungeon_count - 1; j++) {
                dshm->dungeons[j] = dshm->dungeons[j + 1];
            }
            dshm->dungeon_count--;
            return;
        }
    }
    printf("Dungeon tidak ditemukan!\n");
}

void battle_hunter(Hunter *hunter, HunterShared *hshm, char *opponent_name) {
    if (hunter->banned) {
        printf("Kamu dibanned dari battle!\n");
        return;
    }
    for (int i = 0; i < hshm->hunter_count; i++) {
        if (strcmp(hshm->hunters[i].name, opponent_name) == 0 && strcmp(hshm->hunters[i].name, hunter->name) != 0) {
            Hunter *opponent = &hshm->hunters[i];
            int my_stats = hunter->atk + hunter->hp + hunter->def;
            int opp_stats = opponent->atk + opponent->hp + opponent->def;
            if (my_stats > opp_stats) {
                hunter->atk += opponent->atk;
                hunter->hp += opponent->hp;
                hunter->def += opponent->def;
                printf("Kamu menang! Mendapat stats lawan.\n");
                // Hapus lawan
                for (int j = i; j < hshm->hunter_count - 1; j++) {
                    hshm->hunters[j] = hshm->hunters[j + 1];
                }
                hshm->hunter_count--;
            } else {
                opponent->atk += hunter->atk;
                opponent->hp += hunter->hp;
                opponent->def += hunter->def;
                printf("Kamu kalah! Stats diberikan ke lawan.\n");
                // Hapus hunter
                for (int j = 0; j < hshm->hunter_count; j++) {
                    if (strcmp(hshm->hunters[j].name, hunter->name) == 0) {
                        for (int k = j; k < hshm->hunter_count - 1; k++) {
                            hshm->hunters[k] = hshm->hunters[k + 1];
                        }
                        hshm->hunter_count--;
                        break;
                    }
                }
                exit(0); // Keluar karena hunter kalah
            }
            return;
        }
    }
    printf("Lawan tidak ditemukan atau kamu tidak bisa bertarung dengan diri sendiri!\n");
}

void dungeon_notification(DungeonShared *dshm, int level) {
    int iterations = 5; // Display updates 5 times, then return to menu
    for (int i = 0; i < iterations; i++) {
        system("clear");
        printf("=== Dungeon Notifications (Update %d/%d) ===\n", i + 1, iterations);
        show_dungeons_for_level(dshm, level);
        sleep(3);
    }
    printf("Notifications ended. Returning to menu...\n");
}

int main() {
    // Koneksi ke shared memory
    int shm_hunter_id = shmget(SHM_HUNTER_KEY, sizeof(HunterShared), 0666);
    int shm_dungeon_id = shmget(SHM_DUNGEON_KEY, sizeof(DungeonShared), 0666);
    if (shm_hunter_id < 0 || shm_dungeon_id < 0) {
        printf("Sistem belum berjalan!\n");
        exit(1);
    }
    HunterShared *hshm = (HunterShared *)shmat(shm_hunter_id, NULL, 0);
    DungeonShared *dshm = (DungeonShared *)shmat(shm_dungeon_id, NULL, 0);

    Hunter *hunter = NULL;
    printf("1. Login\n2. Register\nPilih: ");
    int choice;
    scanf("%d", &choice);
    char password[50], name[50];
    if (choice == 1) {
        printf("Masukkan nama: ");
        scanf("%s", name);
        printf("Masukkan password: ");
        scanf("%s", password);
        if (!login(hshm, name, password, &hunter)) {
            shmdt(hshm);
            shmdt(dshm);
            return 1;
        }
    } else if (choice == 2) {
        printf("Masukkan nama: ");
        scanf("%s", name);
        printf("Masukkan password: ");
        scanf("%s", password);
        register_hunter(hshm, name, password);
        login(hshm, name, password, &hunter);
    }

    while (1) {
        printf("\nMenu Hunter:\n1. Lihat Dungeon\n2. Taklukkan Dungeon\n3. Battle Hunter\n4. Aktifkan Notifikasi\n5. Keluar\nPilih: ");
        scanf("%d", &choice);
        if (choice == 5) break;

        switch (choice) {
            case 1:
                show_dungeons_for_level(dshm, hunter->level);
                break;
            case 2: {
                char dungeon_key[50];
                printf("Masukkan key dungeon: ");
                scanf("%s", dungeon_key);
                conquer_dungeon(hunter, dshm, dungeon_key);
                break;
            }
            case 3: {
                char opponent_name[50];
                printf("Masukkan nama lawan: ");
                scanf("%s", opponent_name);
                battle_hunter(hunter, hshm, opponent_name);
                break;
            }
            case 4:
                dungeon_notification(dshm, hunter->level);
                break;
        }
    }

    shmdt(hshm);
    shmdt(dshm);
    return 0;
}
