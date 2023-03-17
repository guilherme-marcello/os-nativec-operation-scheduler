#include "../include/memory.h"
#include "../include/memory-private.h"
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

void destroy_shared_memory(char* name, void* ptr, int size) {
    // generate access key
    key_t access_key = ftok(name, 'R');
    // get the id of the shm segment
    int shmid = shmget(access_key, size, 0666);
    // detach shmem
    shmdt(ptr);
    // remove shmem
    shmctl(shmid, IPC_RMID, NULL);
}

void* create_shared_memory(char* name, int size) {
    // generate access key as namekey concatenation
    //uid_t uid = getuid();
    //char key_as_string[64];
    //sprintf(key_as_string, "%s%d", name, uid);
    key_t access_key = ftok(name, 'R');

    // get the id of the shm segment
    int shmid = shmget(access_key, size, 0666|IPC_CREAT);
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
        // check if the ith position in the buffer is free
        if (buffer->ptrs[i] == FREE_MEM) {
            // write the operation to the buffer and update pointer to 1 (used)
            buffer->buffer[i] = *op;
            buffer->ptrs[i] = USED_MEM;
            return; // exit the function
        }
    }
}

void read_main_client_buffer(struct rnd_access_buffer* buffer, int client_id, int buffer_size, struct operation* op) {
    for (int i = 0; i < buffer_size; i++) {
        // check if the ith position in the buffer is in used
        if (buffer->ptrs[i] == USED_MEM) {
            if (buffer->buffer[i].requesting_client == client_id) {
                // point op to buffered op and update ith position to free
                op = &buffer->buffer[i];
                buffer->ptrs[i] = FREE_MEM;
                return; // exit the function
            }
        }
    }
    // no op available
    op->id = -1;
}

void write_interm_enterp_buffer(struct rnd_access_buffer* buffer, int buffer_size, struct operation* op) {
    for (int i = 0; i < buffer_size; i++) {
        // check if the ith position in the buffer is free
        if (buffer->ptrs[i] == FREE_MEM) {
            // write the operation to the buffer and update pointer to 1 (used)
            buffer->buffer[i] = *op;
            buffer->ptrs[i] = USED_MEM;
            return; // exit the function
        }
    }    
}

void read_interm_enterp_buffer(struct rnd_access_buffer* buffer, int enterp_id, int buffer_size, struct operation* op) {
    for (int i = 0; i < buffer_size; i++) {
        // check if the ith position in the buffer is in used
        if (buffer->ptrs[i] == USED_MEM) {
            if (buffer->buffer[i].requested_enterp == enterp_id) {
                // point op to buffered op and update ith position to free
                op = &buffer->buffer[i];
                buffer->ptrs[i] = FREE_MEM;
                return; // exit the function
            }
        }
    }
    // no op available
    op->id = -1;
}
