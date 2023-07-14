// n wątków (parametr), realizujących tę samą funkcję
// w ramach  swojego działania wątki mają za zadanie inkrementować wartość zmiennej globalnej
// i wypisywać jej wartość na konsolę wraz z informacją identyfikującą wątek dokonujący wypisania
// dostęp do zmiennej zabezpiecz mutexem

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int global_variable = 0;
pthread_mutex_t mutex;

void* increment_and_print(void* thread_id) {
    long tid = (long)thread_id;

    pthread_mutex_lock(&mutex);
    global_variable++;

    printf("Thread %ld: %d\n", tid, global_variable);

    pthread_mutex_unlock(&mutex);
    pthread_exit(NULL);
}

int main(int args, char** argv) {
    int num_threads = atoi(argv[1]);
    pthread_t threads[num_threads];
    pthread_mutex_init(&mutex, NULL);

    for (long i = 0; i < num_threads; i++) {
        pthread_create(&threads[i], NULL, increment_and_print, (void*)i);
    }

    // Oczekiwanie na zakończenie wszystkich wątków
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&mutex);

    return 0;
}
