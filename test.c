#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct YourStruct {
    int intValue;
    int anotherValue;
    //char stringValue[50];
    // Add other members as needed
};

int main() {
    key_t key = ftok("/path/to/your/file", 'R');
    int shm_id = shmget(key, sizeof(struct YourStruct), IPC_CREAT | 0666);
    struct YourStruct sd;
    struct YourStruct *shared_data = &sd;
    (*shared_data).intValue = 32;
    printf("%lli\n", shared_data);
    printf("id : %d\n", shm_id);
    shared_data = (struct YourStruct*) shmat(shm_id, NULL, 0);
    printf("%lli\n", shared_data);
    if(shared_data == NULL){
        printf("null");
    }

    // (*shared_data).intValue = 42;
    // strcpy(shared_data->stringValue, "Hello, Shared Memory!");

    // printf("Value in shared memory: %d\n", shared_data->intValue);
    // printf("String in shared memory: %s\n", shared_data->stringValue);

    // shmdt((void*) shared_data);

    // Optionally remove the shared memory segment
    // shmctl(shm_id, IPC_RMID, NULL);

    return 0;
}
