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
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

void* create_dynamic_memory(int size) {
    return calloc(1, size);
}

void destroy_dynamic_memory(void* ptr) {
    free(ptr);
}

void destroy_shared_memory(char * name, void * ptr, int size) {
    int uid = getuid();
    char name_uid[strlen(name)+10];
    sprintf(name_uid,"%s_%d", name, uid);

    int ret;
    ret = munmap(ptr, size); 
    if(ret == -1){ 
        perror(name); 
        exit(7); 
    } 
    ret = shm_unlink(name); 
    if(ret == -1){ 
        perror(name); 
        exit(8); 
    }
}

void * create_shared_memory(char* name, int size) {
    int uid = getuid();
    char name_uid[strlen(name)+10];
    sprintf(name_uid,"%s_%d", name, uid);

    int *ptr; 
 	int ret; 
 	int fd = shm_open(name, O_RDWR|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR); 
 	if(fd ==-1){ 
 		perror(name); 
 		return NULL;
 	} 
 
 	ret = ftruncate(fd,size); 
 	if (ret==-1){ 
 		perror(name); 
 		return NULL;
 	} 
 	ptr = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0); 
 	if (ptr == MAP_FAILED){ 
 		perror(name); 
 		return NULL;
 	} 
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

void write_client_interm_buffer(struct circular_buffer* buffer, int buffer_size, struct operation* op) {

}

void read_client_interm_buffer(struct circular_buffer* buffer, int buffer_size, struct operation* op) {

}
