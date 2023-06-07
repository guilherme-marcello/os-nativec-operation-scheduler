/**
 * SO-035
 * Guilherme Marcelo    <fc58173>
 * Eduardo Santos       <fc58185>
 * Xi Wang              <fc58183>
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "main.h"
#include "memory.h"
#include "intermediary.h"
#include "main-private.h"


int execute_intermediary(int interm_id, struct comm_buffers* buffers, struct main_data* data) {
    // first, setup op and op_counter
    struct operation op = {0, 0, 0, 0, 0, 0};
    int op_counter = 0;
    while ((*data->terminate) == 0) { // while did not receive terminate flag...
        intermediary_receive_operation(&op, buffers, data); // read op from buffer
        // if there's no op to do
        if (op.id == -1) 
            continue; // skip this iteration
        else
            printf(INFO_RECEIVED_OP, "Intermediary", interm_id, op.id);
        
        // found operation to do! processing it...
        intermediary_process_operation(&op, interm_id, data, &op_counter);
        intermediary_send_answer(&op, buffers, data);

    }
    // return counter of processed operations
    return op_counter;
}

void intermediary_receive_operation(struct operation* op, struct comm_buffers* buffers, struct main_data* data) {
    if (*(data->terminate) == 1) return; // return if program received terminate flag
    // read client-interm buffer
    read_client_interm_buffer(buffers->client_interm, data->buffers_size, op);
}

void intermediary_process_operation(struct operation* op, int interm_id, struct main_data* data, int* counter) {
    op->receiving_interm = interm_id; // update receiving interm field
    op->status = 'I'; // change status to 'I' (processed by interm)
    (*counter)++; // increment counter of this interm's processed operations
    memcpy(&data->results[op->id], op, sizeof(struct operation));
}

void intermediary_send_answer(struct operation* op, struct comm_buffers* buffers, struct main_data* data) {
    write_interm_enterp_buffer(buffers->interm_enterp, data->buffers_size, op);
}