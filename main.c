#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h> 
#include <signal.h>
#include <pthread.h> 
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <bits/mman-linux.h>
#include <sys/wait.h>
sem_t sem1, sem2;
pthread_mutex_t printMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t pMutex = PTHREAD_MUTEX_INITIALIZER;
typedef struct
{
   int count;
} DOTDATA;
DOTDATA dc;
typedef struct
{
    int depth; 
    char path[500];
}t_arg;

int isDir(const char *path) {
    struct stat st;
    
    if (stat(path, &st) == 0) {
        return S_ISDIR(st.st_mode);
    } else {
        // Handle error, e.g., print or log an error message
        perror("stat");
        return -1;  // Indicate failure
    }
}


void *threadFun(void* args){
    
    t_arg *a = (t_arg*)args;
    pthread_mutex_lock(&printMutex);
    //printf("\nthread %s\n", a->path);
    pthread_mutex_unlock(&printMutex);
    directory_task(a->path, a->depth);
    pthread_exit(NULL);
}

void make_path(char parent[], char child[], char *path){
    strcpy(path, parent);
    strcat(path, "/");
    strcat(path, child);
}

void first_task(char dir_address[]){
    struct dirent *de;
    struct dirent* files[500] = {NULL};
    struct dirent* directories[500] = {NULL};
    int files_index = 0;
    int dir_index = 0;
	DIR *dr = opendir(dir_address); 
    
    pid_t pids[200]; //number of process in main directory
    int pids_index = 0;


	if (dr == NULL) // opendir returns NULL if couldn't open directory 
	{ 
		printf("Could not open current directory" );
	} 

	// Refer http://pubs.opengroup.org/onlinepubs/7990989775/xsh/readdir.html 
	// for readdir() 
	while ((de = readdir(dr)) != NULL) {
        if(!strcmp(de->d_name, ".") || !strcmp(de->d_name, "..")){
           continue;
        }
        char path[500];
        strcpy(path, dir_address);
        strcat(path, "/");
        strcat(path, de->d_name);
        if(isDir(path) == 1) {
            directories[dir_index] = de;
            dir_index++;
        } else {
            files[files_index] = de;
            dc.count++;
            files_index++;
        }
        char empty[] = "";
        strcpy(path, empty);
    }
    printf("-----------------\n\n");
    // for (int i = 0; i<dir_index; i++) {
    //     printf("%s\n", directories[i]->d_name);
    // }
    // printf("-----------------\n\n");
    // for (int i = 0; i<files_index; i++) {
    //     printf("%s\n", files[i]->d_name);
    // }

    for (int i = 0; i < dir_index; i++) {
        pthread_mutex_lock(&printMutex);
        printf("%s\n", directories[i]->d_name);
        pthread_mutex_unlock(&printMutex);
        pid_t pid = fork(); // Create a new process
        if (pid < 0) {
            // Fork failed
            fprintf(stderr, "Fork failed\n");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            // This code is executed by the child process

            printf("Child process %d is running\n", i);
            char path[500];
            strcpy(path, dir_address);
            strcat(path, "/");
            strcat(path, directories[i]->d_name);
            directory_task(path, 1);

            // Perform a different task for each child process
            // For example, you can use the value of 'i' to determine the task

            exit(EXIT_SUCCESS); // Terminate the child process
        }
        int returnStatus;    
        waitpid(pid, &returnStatus, 0);
    }

    // This code is executed by the parent process
    // Parent process may perform some other tasks or wait for child processes

    for (int i = 0; i < dir_index; i++) {
        wait(NULL); // Wait for each child process to finish
    }
    pthread_mutex_lock(&printMutex);
    for (int i = 0; i<files_index; i++) {
        printf("%s\n", files[i]->d_name);
    }
    pthread_mutex_unlock(&printMutex);
    //printf("All child processes have terminated\n");

	closedir(dr);	 
	return; 
}


void directory_task(char dir_address[], int depth){
    struct dirent *de;
    struct dirent* files[500] = {NULL};
    struct dirent* directories[500] = {NULL};
    int files_index = 0;
    int dir_index = 0;
	DIR *dr = opendir(dir_address); 
    pthread_t threads[500];
    int thread_args[500];

	if (dr == NULL) // opendir returns NULL if couldn't open directory 
	{ 
		printf("Could not open current directory" );
	} 

	
	while ((de = readdir(dr)) != NULL) {
        if(!strcmp(de->d_name, ".") || !strcmp(de->d_name, "..")){
           continue;
        }
        char path[500];
        make_path(dir_address, de->d_name, path);
        if(isDir(path) == 1) {
            pthread_mutex_lock(&printMutex);
            sem_wait(&sem1);
            for(int i=0;i<depth; i++){
                printf("\t");
            }
            printf("%s\n", de->d_name);
            pthread_mutex_unlock(&printMutex);
            directories[dir_index] = de;
            thread_args[dir_index] = dir_index;
            //char *args = path;
            t_arg args;
            strcpy(args.path, path);
            args.depth = depth + 1;
            pthread_create(&threads[dir_index], NULL, threadFun, &args);
            pthread_join(threads[dir_index], NULL);
            //dc.count++;
            //printf("count%d\n", dc.count);
            dir_index++;
            sem_post(&sem1);
            //free(args);
        } else {
            files[files_index] = de;
            dc.count++;
            printf("count%d\n", dc.count);
            files_index++;
        }
        char empty[] = "";
        strcpy(path, empty);
    }
    //printf("fffffffffff%d\n", dir_index);
    // for(int i=0; i<dir_index; i++){
    //     pthread_join(threads[i], NULL);
    // }
    // for (int i = 0; i<dir_index; i++) {
    //     printf("%s\n", directories[i]->d_name);
    // }
    pthread_mutex_lock(&printMutex);
    printf("\n-----------------\n\n");
    for (int i = 0; i<files_index; i++) {
        for(int j=0;j<depth; j++){
            printf("\t");
        }
        printf("%s\n", files[i]->d_name);
    }
    pthread_mutex_unlock(&printMutex);
    //pthread_exit(NULL);
	closedir(dr);	 
	return;
}


int main(void) 
{ 
    char dir_address[500];
    printf("enter");
    scanf("%s", dir_address);
    sem_init(&sem1, 0, 1);
    //sem_init(&sem2, 0, 1);
    // int* p = (int*) mmap(NULL, sizeof (int) , PROT_READ | PROT_WRITE,
    // MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    // *p = 0;
    pthread_mutex_lock(&pMutex);
    first_task(dir_address);
    pthread_mutex_unlock(&pMutex);
    printf("%d\n", dc.count);
    return 0;
} 

