#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>
#include <time.h>
#include <unistd.h>

#define SHM_HUNTER_KEY 1234
#define SHM_DUNGEON_KEY 5678
#define MAX_HUNTERS 100
#define MAX_DUNGEONS 50

// Struktur data (sama seperti di atas)
typedef struct {
    char key[50];
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

void create_dungeon(DungeonShared *dshm) {
    if (dshm->dungeon_count >= MAX_DUNGEONS) {
        printf("Dungeon penuh!\n");
        return;
    }
    Dungeon *d = &dshm->dungeons[dshm->dungeon_count];
    sprintf(d->name, "Dungeon-%d", dshm->dungeon_count + 1);
    d->min_level = rand() % 5 + 1;
    d->reward_atk = rand() % 51 + 100;
    d->reward_hp = rand() % 51 + 50;
    d->reward_def = rand() % 26 + 25;
    d->reward_exp = rand() % 151 + 150;
    sprintf(d->key, "DUN-%d", dshm->dungeon_count + 1);
    dshm->dungeon_count++;
    printf("Dungeon %s dibuat!\n", d->name);
}

void show_hunters(HunterShared *hshm) {
    printf("Daftar Hunter:\n");
    for (int i = 0; i < hshm->hunter_count; i++) {
        Hunter *h = &hshm->hunters[i];
        printf("Key: %s, Nama: %s, Level: %d, EXP: %d, ATK: %d, HP: %d, DEF: %d, Banned: %d\n",
               h->key, h->name, h->level, h->exp, h->atk, h->hp, h->def, h->banned);
    }
}

void show_dungeons(DungeonShared *dshm) {
    printf("Daftar Dungeon:\n");
    for (int i = 0; i < dshm->dungeon_count; i++) {
        Dungeon *d = &dshm->dungeons[i];
        printf("Key: %s, Nama: %s, Min Level: %d, Reward: ATK=%d, HP=%d, DEF=%d, EXP=%d\n",
               d->key, d->name, d->min_level, d->reward_atk, d->reward_hp, d->reward_def, d->reward_exp);
    }
}

void ban_hunter(HunterShared *hshm, char *key, int ban) {
    for (int i = 0; i < hshm->hunter_count; i++) {
        if (strcmp(hshm->hunters[i].key, key) == 0) {
            hshm->hunters[i].banned = ban;
            printf("Hunter %s %s!\n", hshm->hunters[i].name, ban ? "dibanned" : "diunbanned");
            return;
        }
    }
    printf("Hunter tidak ditemukan!\n");
}

void reset_hunter(HunterShared *hshm, char *key) {
    for (int i = 0; i < hshm->hunter_count; i++) {
        if (strcmp(hshm->hunters[i].key, key) == 0) {
            Hunter *h = &hshm->hunters[i];
            h->level = 1;
            h->exp = 0;
            h->atk = 10;
            h->hp = 100;
            h->def = 5;
            printf("Stats hunter %s direset!\n", h->name);
            return;
        }
    }
    printf("Hunter tidak ditemukan!\n");
}

int main() {
    srand(time(NULL));

    // Inisialisasi shared memory
    int shm_hunter_id = shmget(SHM_HUNTER_KEY, sizeof(HunterShared), IPC_CREAT | 0666);
    int shm_dungeon_id = shmget(SHM_DUNGEON_KEY, sizeof(DungeonShared), IPC_CREAT | 0666);
    HunterShared *hshm = (HunterShared *)shmat(shm_hunter_id, NULL, 0);
    DungeonShared *dshm = (DungeonShared *)shmat(shm_dungeon_id, NULL, 0);

    // Inisialisasi counter
    hshm->hunter_count = 0;
    dshm->dungeon_count = 0;

    while (1) {
        printf("\nMenu Sistem:\n1. Tampilkan Hunter\n2. Tampilkan Dungeon\n3. Buat Dungeon\n4. Ban Hunter\n5. Unban Hunter\n6. Reset Hunter\n7. Keluar\nPilih: ");
        int choice;
        scanf("%d", &choice);
        if (choice == 7) break;

        switch (choice) {
            case 1:
                show_hunters(hshm);
                break;
            case 2:
                show_dungeons(dshm);
                break;
            case 3:
                create_dungeon(dshm);
                break;
            case 4: {
                char key[50];
                printf("Masukkan key hunter: ");
                scanf("%s", key);
                ban_hunter(hshm, key, 1);
                break;
            }
            case 5: {
                char key[50];
                printf("Masukkan key hunter: ");
                scanf("%s", key);
                ban_hunter(hshm, key, 0);
                break;
            }
            case 6: {
                char key[50];
                printf("Masukkan key hunter: ");
                scanf("%s", key);
                reset_hunter(hshm, key);
                break;
            }
        }
    }

    // Pembersihan shared memory
    shmdt(hshm);
    shmdt(dshm);
    shmctl(shm_hunter_id, IPC_RMID, NULL);
    shmctl(shm_dungeon_id, IPC_RMID, NULL);
    printf("Sistem dimatikan, shared memory dihapus.\n");
    return 0;
}
