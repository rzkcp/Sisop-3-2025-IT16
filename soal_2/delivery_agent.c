#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <time.h>

#define SHM_NAME "/rushgo_shared_memory"
#define MAX_ORDER 100

typedef struct {
    char nama[50];
    char alamat[100];
    char jenis[10];  // "Express" atau "Reguler"
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

    fprintf(logfile, "[%02d/%02d/%04d %02d:%02d:%02d] [%s] Express package delivered to %s in %s\n",
        t->tm_mday, t->tm_mon + 1, t->tm_year + 1900,
        t->tm_hour, t->tm_min, t->tm_sec,
        agent, nama, alamat
    );

    fclose(logfile);
}

void* agent_thread(void* arg) {
    char* agent_name = (char*) arg;

    while (1) {
        pthread_mutex_lock(&shared_data->mutex);
        for (int i = 0; i < shared_data->count; ++i) {
            if (strcmp(shared_data->orders[i].jenis, "Express") == 0 &&
                strcmp(shared_data->orders[i].status, "Pending") == 0) {
                
                // Tandai sebagai delivered
                snprintf(shared_data->orders[i].status, 100, "Delivered by %s", agent_name);

                // Tulis log
                log_delivery(agent_name,
                    shared_data->orders[i].nama,
                    shared_data->orders[i].alamat);
                break;
            }
        }
        pthread_mutex_unlock(&shared_data->mutex);

        sleep(1); // delay supaya tidak loop terlalu cepat
    }

    return NULL;
}

int main() {
    int shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
    ftruncate(shm_fd, sizeof(SharedData));
    shared_data = mmap(0, sizeof(SharedData), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    pthread_t agentA, agentB, agentC;
    pthread_create(&agentA, NULL, agent_thread, "AGENT A");
    pthread_create(&agentB, NULL, agent_thread, "AGENT B");
    pthread_create(&agentC, NULL, agent_thread, "AGENT C");

    pthread_join(agentA, NULL);
    pthread_join(agentB, NULL);
    pthread_join(agentC, NULL);

    return 0;
}

