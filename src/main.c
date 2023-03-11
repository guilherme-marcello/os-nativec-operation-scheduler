#include "../include/main.h"
#include <memory.h>


int main(int argc, char *argv[]) {
    //init data structures
    struct main_data* data = create_dynamic_memory(sizeof(struct main_data));
    struct comm_buffers* buffers = create_dynamic_memory(sizeof(struct comm_buffers));

    buffers->main_client = create_dynamic_memory(sizeof(struct circular_buffer));
    buffers->client_interm = create_dynamic_memory(sizeof(struct rnd_access_buffer));
    buffers->interm_enterp = create_dynamic_memory(sizeof(struct circular_buffer));

    //execute main code
    main_args(argc, argv, data);
    
    create_dynamic_memory_buffers(data);    
    create_shared_memory_buffers(data, buffers);
    launch_processes(buffers, data);
    user_interaction(buffers, data);
    
    //release memory before terminating
    destroy_dynamic_memory(data);
    destroy_dynamic_memory(buffers->main_client);
    destroy_dynamic_memory(buffers->client_interm);
    destroy_dynamic_memory(buffers->interm_enterp);
    destroy_dynamic_memory(buffers);
}
