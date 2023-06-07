/**
 * SO-035
 * Guilherme Marcelo    <fc58173>
 * Eduardo Santos       <fc58185>
 * Xi Wang              <fc58183>
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "client.h"
#include "memory.h"
#include "main-private.h"


int execute_client(int client_id, struct comm_buffers* buffers, struct main_data* data) {
    // first, setup op and op_counter
    struct operation op = {0, 0, 0, 0, 0, 0};
    int op_counter = 0;
    while ((*data->terminate) == 0) { // while did not receive terminate flag...
        client_get_operation(&op, client_id, buffers, data); // read op from buffer
        // if there's no op to do
        if (op.id == -1)
            continue; // skip this iteration
        else
            printf(INFO_RECEIVED_OP, "Client", client_id, op.id);
    
        // found operation to do! processing it...
        client_process_operation(&op, client_id, data, &op_counter);
        client_send_operation(&op, buffers, data);
    }
    // return counter of processed operations
    return op_counter;
}

void client_get_operation(struct operation* op, int client_id, struct comm_buffers* buffers, struct main_data* data) {
    if (*(data->terminate) == 1) return; // return if program received terminate flag
    // read main-client buffer
    read_main_client_buffer(buffers->main_client, client_id, data->buffers_size, op);
}

void client_process_operation(struct operation* op, int client_id, struct main_data* data, int* counter) {
    op->receiving_client = client_id; // update receiving client field
    op->status = 'C'; // change status to 'C' (processed by client)
    (*counter)++; // increment counter of this client's processed operations
    memcpy(&data->results[op->id], op, sizeof(struct operation));
}

void client_send_operation(struct operation* op, struct comm_buffers* buffers, struct main_data* data) {
    // send operation to interm buffer
    write_client_interm_buffer(buffers->client_interm, data->buffers_size, op);
}