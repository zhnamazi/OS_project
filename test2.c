#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
    key_t key = ftok("/path/to/your/file", 'R');

    // Create or get the shared memory segment with read and write permissions (0666)
    int shm_id = shmget(key, sizeof(int), IPC_CREAT | 0666);

    // Attach the shared memory segment to the process
    int *shared_data = (int*) shmat(shm_id, NULL, 0);

    // Use the shared memory variable
    *shared_data = 42;

    // Print the value from shared memory
    printf("Value in shared memory: %d\n", *shared_data);

    // Detach the shared memory segment
    shmdt(shared_data);

    // Optionally remove the shared memory segment
    // shmctl(shm_id, IPC_RMID, NULL);

    return 0;
}
