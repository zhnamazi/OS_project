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

typedef struct
{
    int *tid; 
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
    t_arg *cur_args = args;
    printf("\nthread %d\n", *(cur_args->tid));
    printf("%s\n", cur_args->path);
   // directory_task(path);
   free(cur_args);
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
            files_index++;
        }
        char empty[] = "";
        strcpy(path, empty);
    }
    printf("-----------------\n\n");
    for (int i = 0; i<dir_index; i++) {
        printf("%s\n", directories[i]->d_name);
    }
    printf("-----------------\n\n");
    for (int i = 0; i<files_index; i++) {
        printf("%s\n", files[i]->d_name);
    }

    for (int i = 0; i < dir_index; i++) {
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
            directory_task(path);

            // Perform a different task for each child process
            // For example, you can use the value of 'i' to determine the task

            exit(EXIT_SUCCESS); // Terminate the child process
        }
    }

    // This code is executed by the parent process
    // Parent process may perform some other tasks or wait for child processes

    for (int i = 0; i < dir_index; i++) {
        wait(NULL); // Wait for each child process to finish
    }

    printf("All child processes have terminated\n");


	closedir(dr);	 
	return; 
}


void directory_task(char dir_address[]){
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
            directories[dir_index] = de;
            thread_args[dir_index] = dir_index;
            t_arg* args = malloc(sizeof *args);
            args->tid = &dir_index;
            strcpy(args->path, path);
            pthread_create(&threads[dir_index], NULL, threadFun, args);
            dir_index++;
            free(args);
        } else {
            files[files_index] = de;
            files_index++;
        }
        char empty[] = "";
        strcpy(path, empty);
    }
    //printf("fffffffffff%d\n", dir_index);
    for(int i=0; i<dir_index; i++){
        pthread_join(threads[i], NULL);
    }
    for (int i = 0; i<dir_index; i++) {
        printf("%s\n", directories[i]->d_name);
    }
    printf("\n-----------------\n\n");
    for (int i = 0; i<files_index; i++) {
        printf("%s\n", files[i]->d_name);
    }

    
	closedir(dr);	 
	return;
}


int main(void) 
{ 
    char dir_address[500];
    printf("enter");
    scanf("%s", dir_address);
    first_task(dir_address);
    return 0;
} 
