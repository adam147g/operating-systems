#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_REINDEER 9
#define NUM_DELIVERIES 3

pthread_mutex_t santa_mutex;
pthread_cond_t reindeer_cond;
pthread_cond_t santa_cond;

int reindeer_count = 0;
int deliveries_made = 0;
int delivering;

pthread_t reindeer_tids[NUM_REINDEER];

void *reindeer_thread(void *arg) {
    int reindeer_id = *(int *)arg;
    sleep(rand() % 6 + 5);
    
    pthread_mutex_lock(&santa_mutex);
    printf("Renifer: czeka %d reniferów na Mikołaja, ID: %d\n", reindeer_count + 1, reindeer_id);
    reindeer_count++;
    
    if (reindeer_count == NUM_REINDEER) {
        printf("Renifer: wybudzam Mikołaja, ID: %d\n", reindeer_id);
        pthread_cond_broadcast(&reindeer_cond);
    }
    
    pthread_cond_wait(&santa_cond, &santa_mutex);
    pthread_mutex_unlock(&santa_mutex);
    
    printf("Renifer: dostarczam zabawki, ID: %d\n", reindeer_id);
    sleep(delivering);
    printf("Renifer: koniec na dziś, ID: %d\n", reindeer_id);
    
    pthread_exit(NULL);
}

void *santa_thread(void *arg) {
    while (deliveries_made < NUM_DELIVERIES) {
        int reindeer_ids[NUM_REINDEER];
        for (int i = 0; i < NUM_REINDEER; i++) {
            reindeer_ids[i] = i + 1;
            pthread_create(&reindeer_tids[i], NULL, reindeer_thread, &reindeer_ids[i]);}
        pthread_mutex_lock(&santa_mutex);
        
        while (reindeer_count < NUM_REINDEER) {
            printf("Mikołaj: śpi.\n");
            pthread_cond_wait(&reindeer_cond, &santa_mutex);
        }
        
        printf("Mikołaj: budzę się.\n");
        delivering = rand() % 3 + 2;
        sleep(delivering);
        printf("Mikołaj: dostarczam zabawki.\n");
        deliveries_made++;
        
        reindeer_count = 0;
        pthread_cond_broadcast(&santa_cond);
        pthread_mutex_unlock(&santa_mutex);
        
        for (int i = 0; i < NUM_REINDEER; i++) {
            pthread_join(reindeer_tids[i], NULL);
        }
        printf("Mikołaj: zasypiam.\n");
        printf("-------------------------------------\n");

    }
    
    pthread_exit(NULL);
}


int main() {
    srand(time(NULL));

    pthread_mutex_init(&santa_mutex, NULL);
    pthread_cond_init(&reindeer_cond, NULL);
    pthread_cond_init(&santa_cond, NULL);

    pthread_t santa_tid;
    pthread_create(&santa_tid, NULL, santa_thread, NULL);

    pthread_join(santa_tid, NULL);

    pthread_mutex_destroy(&santa_mutex);
    pthread_cond_destroy(&reindeer_cond);
    pthread_cond_destroy(&santa_cond);
    
    return 0;
}
