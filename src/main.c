#include "../include/main.h"
#include "../include/main-private.h"
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void verify_condition(int condition, char* snippet_id, char* error_msg) {
    if (condition) {
        fprintf(stderr, "[%s] %s", snippet_id, error_msg);
        exit(EXIT_FAILURE);        
    }
}

int min(int a, int b) {
    return (a < b) ? a : b;
}

void main_args(int argc, char* argv[], struct main_data *data) { 
    data->max_ops = atoi(argv[1]);
    data->buffers_size = atoi(argv[2]);
    data->n_clients = atoi(argv[3]);
    data->n_intermediaries = atoi(argv[4]);
    data->n_enterprises = atoi(argv[5]);
}

void create_dynamic_memory_buffers(struct main_data *data) {
    data->client_pids = create_dynamic_memory(sizeof(uid_t) * data->n_clients);
    data->intermediary_pids = create_dynamic_memory(sizeof(uid_t) * data->n_intermediaries);
    data->enterprise_pids = create_dynamic_memory(sizeof(uid_t) * data->n_enterprises);
    data->client_stats = create_dynamic_memory(sizeof(int) * data->n_clients);
    data->intermediary_stats = create_dynamic_memory(sizeof(int) * data->n_intermediaries);
    data->enterprise_stats = create_dynamic_memory(sizeof(int) * data->n_enterprises);
}

void create_shared_memory_buffers(struct main_data *data, struct comm_buffers *buffers) {
    // comm buffers
    buffers->main_client->ptrs = create_shared_memory(
        STR_SHM_MAIN_CLIENT_PTR, 
        data->buffers_size
    );

    buffers->main_client->buffer = create_shared_memory(
        STR_SHM_MAIN_CLIENT_BUFFER, 
        data->buffers_size
    );

    buffers->client_interm->ptrs = create_shared_memory(
        STR_SHM_CLIENT_INTERM_PTR, 
        data->buffers_size
    );

    buffers->client_interm->buffer = create_shared_memory(
        STR_SHM_CLIENT_INTERM_BUFFER, 
        data->buffers_size
    );

    buffers->interm_enterp->ptrs = create_shared_memory(
        STR_SHM_INTERM_ENTERP_PTR, 
        data->buffers_size
    );

    buffers->interm_enterp->buffer = create_shared_memory(
        STR_SHM_INTERM_ENTERP_BUFFER, 
        data->buffers_size
    );

    // main_data-related fields
    data->results = create_shared_memory(
        STR_SHM_RESULTS, 
        min(data->buffers_size, sizeof(struct operation) * MAX_RESULTS)
    ); 
    data->terminate = create_shared_memory(
        STR_SHM_TERMINATE, 
        min(data->buffers_size, sizeof(int))
    );
}

int main(int argc, char *argv[]) {
    //init data structures
    struct main_data* data = create_dynamic_memory(sizeof(struct main_data));
    struct comm_buffers* buffers = create_dynamic_memory(sizeof(struct comm_buffers));
    verify_condition(!data || !buffers, INIT_DS ,ERROR_MALLOC);

    buffers->main_client = create_dynamic_memory(sizeof(struct rnd_access_buffer));
    buffers->client_interm = create_dynamic_memory(sizeof(struct circular_buffer));
    buffers->interm_enterp = create_dynamic_memory(sizeof(struct rnd_access_buffer));
    verify_condition(
        !buffers->main_client || !buffers->client_interm || !buffers->interm_enterp, 
        INIT_COMM_BUFFER,
        ERROR_MALLOC
    );

    //execute main code
    verify_condition(
        argc != NUMBER_OF_ARGS,
        INIT_MAIN,
        ERROR_ARGS
    );
    main_args(argc, argv, data);
    
    create_dynamic_memory_buffers(data);
    verify_condition(
        !data->client_pids || !data->intermediary_pids || !data->enterprise_pids 
        || !data->client_stats || !data->intermediary_stats || !data->enterprise_stats, 
        INIT_DMEM_BUFFERS,
        ERROR_MALLOC
    );
    
    create_shared_memory_buffers(data, buffers);
    verify_condition(
        !data->results || !data->terminate 
        || !buffers->main_client->ptrs || !buffers->main_client->buffer
        || !buffers->client_interm->ptrs || !buffers->client_interm->buffer
        || !buffers->interm_enterp->ptrs || !buffers->interm_enterp->buffer, 
        INIT_SHMEM_BUFFERS,
        ERROR_MALLOC
    );

    //launch_processes(buffers, data);
    //user_interaction(buffers, data);
    
    //release memory before terminating
    destroy_dynamic_memory(data);
    destroy_dynamic_memory(buffers->main_client);
    destroy_dynamic_memory(buffers->client_interm);
    destroy_dynamic_memory(buffers->interm_enterp);
    destroy_dynamic_memory(buffers);
}
