#include <stdio.h>
#include <pthread.h>

// Function to be executed by all threads
void* threadFunction(void* arg) {
    int tid = *((int*)arg);
    printf("Hello from thread %d\n", tid);
    return NULL;
}

int main() {
    pthread_t threads[5];
    int thread_args[5];
    
    // Create and run each thread
    for (int i = 0; i < 5; i++) {
        thread_args[i] = i;
        if (pthread_create(&threads[i], NULL, threadFunction, &thread_args[i]) != 0) {
            perror("pthread_create");
            return 1;
        }
    }

    // Wait for all threads to complete
    for (int i = 0; i < 5; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            perror("pthread_join");
            return 1;
        }
    }

    return 0;
}