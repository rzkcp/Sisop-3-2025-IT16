#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <pthread.h>
#include <time.h>

#define SHM_NAME "/rushgo_shared_memory"
#define MAX_ORDER 100

typedef struct {
    char nama[50];
    char alamat[100];
    char jenis[10];
    char status[100];
} Order;

typedef struct {
    Order orders[MAX_ORDER];
    int count;
    pthread_mutex_t mutex;
} SharedData;

SharedData *shared_data;

void log_delivery(const char* agent, const char* nama, const char* alamat) {
    FILE *logfile = fopen("delivery.log", "a");
    if (!logfile) return;

    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    fprintf(logfile, "[%02d/%02d/%04d %02d:%02d:%02d] [AGENT %s] Reguler package delivered to %s in %s\n",
        t->tm_mday, t->tm_mon + 1, t->tm_year + 1900,
        t->tm_hour, t->tm_min, t->tm_sec,
        agent, nama, alamat
    );

    fclose(logfile);
}

void load_orders_from_csv(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Gagal membuka file CSV");
        exit(1);
    }

    int index = 0;
    char line[256];

    while (fgets(line, sizeof(line), file)) {
        if (index >= MAX_ORDER) break;
        char *nama = strtok(line, ",");
        char *alamat = strtok(NULL, ",");
        char *jenis = strtok(NULL, "\n");

        strcpy(shared_data->orders[index].nama, nama);
        strcpy(shared_data->orders[index].alamat, alamat);
        strcpy(shared_data->orders[index].jenis, jenis);
        strcpy(shared_data->orders[index].status, "Pending");

        index++;
    }

    shared_data->count = index;
    fclose(file);
}

void deliver_order(const char *nama, const char *agent_name) {
    pthread_mutex_lock(&shared_data->mutex);
    for (int i = 0; i < shared_data->count; i++) {
        if (strcmp(shared_data->orders[i].nama, nama) == 0 &&
            strcmp(shared_data->orders[i].jenis, "Reguler") == 0 &&
            strcmp(shared_data->orders[i].status, "Pending") == 0) {

            snprintf(shared_data->orders[i].status, 100, "Delivered by Agent %s", agent_name);
            log_delivery(agent_name, shared_data->orders[i].nama, shared_data->orders[i].alamat);
            printf("Pesanan %s berhasil dikirim oleh Agent %s\n", nama, agent_name);
            pthread_mutex_unlock(&shared_data->mutex);
            return;
        }
    }
    pthread_mutex_unlock(&shared_data->mutex);
    printf("Pesanan tidak ditemukan atau sudah dikirim.\n");
}

void check_status(const char *nama) {
    for (int i = 0; i < shared_data->count; i++) {
        if (strcmp(shared_data->orders[i].nama, nama) == 0) {
            printf("Status for %s: %s\n", nama, shared_data->orders[i].status);
            return;
        }
    }
    printf("Pesanan %s tidak ditemukan.\n", nama);
}

void list_orders() {
    printf("Daftar Pesanan:\n");
    for (int i = 0; i < shared_data->count; i++) {
        printf("- %s: %s\n", shared_data->orders[i].nama, shared_data->orders[i].status);
    }
}

int main(int argc, char *argv[]) {
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, sizeof(SharedData));
    shared_data = mmap(0, sizeof(SharedData), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(&shared_data->mutex, &attr);

    if (argc == 1) {
        load_orders_from_csv("delivery_order.csv");
        printf("Data pesanan berhasil dimuat ke shared memory.\n");
        return 0;
    }

    if (strcmp(argv[1], "-deliver") == 0 && argc == 3) {
        char *username = getenv("USER");
        deliver_order(argv[2], username ? username : "USER");
    } else if (strcmp(argv[1], "-status") == 0 && argc == 3) {
        check_status(argv[2]);
    } else if (strcmp(argv[1], "-list") == 0) {
        list_orders();
    } else {
        printf("Penggunaan:\n");
        printf("./dispatcher -deliver [Nama]\n");
        printf("./dispatcher -status [Nama]\n");
        printf("./dispatcher -list\n");
    }

    return 0;
}
