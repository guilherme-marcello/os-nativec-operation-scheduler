#include "../include/memory.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <sys/shm.h>

void* create_dynamic_memory(int size) {
    return calloc(1, size);
}

void destroy_dynamic_memory(void* ptr) {
    free(ptr);
}

void* create_shared_memory(char* name, int size) {
    // generate access key as namekey concatenation
    uid_t uid = getuid();
    char key_as_string[64];
    sprintf(key_as_string, "%s%d", name, uid);
    key_t access_key = ftok(key_as_string, 'R');

    // get the id of the shm segment
    int shmid = shmget(access_key, size, 0644|IPC_CREAT);
    // attach to the shm segment, getting a pointer
    void* ptr = shmat(shmid, NULL, 0);
    // fill segment with 0
    memset(ptr, 0, size);
    // detach and return pointer
    shmdt(ptr);
    return ptr;
}

void write_main_client_buffer(struct rnd_access_buffer* buffer, int buffer_size, struct operation* op) {
    for (int i = 0; i < buffer_size; i++) {
        // check if the current position in the buffer is free
        if (buffer->ptrs[i] == 0) {
            // write the operation to the buffer and update pointer to 1 (used)
            buffer->buffer[i] = *op;
            buffer->ptrs[i] = 1;
            return; // exit the function
        }
    }
}