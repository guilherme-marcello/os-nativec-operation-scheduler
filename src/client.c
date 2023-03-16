#include <stdlib.h>
#include "../include/client.h"
#include "../include/memory.h"


int execute_client(int client_id, struct comm_buffers* buffers, struct main_data* data) {
    // first, setup op and op_counter
    struct operation* op;
    int op_counter = 0;
    while (data->terminate == 0) { // while did not receive terminate flag...
        op = NULL; // every new iteration, op should be set to NULL
        client_get_operation(op, client_id, buffers, data); // read op from buffer
        if (op == NULL || op->id == -1) // if there's no op to do or get operation did not succeed
            continue; // skip this iteration
        // found operation to do! processing it...
        client_process_operation(op, client_id, data, &op_counter);
        client_send_operation(op, buffers, data);

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
    // TODO: what do you mean by "Atualiza também a operação na estrutura data"?
}

void client_send_operation(struct operation* op, struct comm_buffers* buffers, struct main_data* data) {
    // send operation to interm buffer
    //write_client_interm_buffer(buffers->client_interm, data->buffers_size, op);
}
