#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// Function executed by the first thread
void* threadFunction1(void* arg) {
    for (int i = 0; i < 5; i++) {
        printf("Thread 1: %d\n", i);
    }
    return NULL;
}

// Function executed by the second thread
void* threadFunction2(void* arg) {
    for (int i = 0; i < 5; i++) {
        printf("Thread 2: %d\n", i);
    }
    return NULL;
}

int main() {
    pthread_t tid1, tid2; // Thread IDs
    if (pthread_create(&tid1, NULL, threadFunction1, NULL) != 0) {
        perror("pthread_create");
        exit(1);
    }
    if (pthread_create(&tid2, NULL, threadFunction2, NULL) != 0) {
        perror("pthread_create");
        exit(1);
    }
    pthread_join(tid1, NULL); // Wait for the first thread to finish
    pthread_join(tid2, NULL); // Wait for the second thread to finish
    return 0;
}

