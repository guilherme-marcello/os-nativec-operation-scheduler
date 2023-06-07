/**
 * SO-035
 * Guilherme Marcelo    <fc58173>
 * Eduardo Santos       <fc58185>
 * Xi Wang              <fc58183>
*/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "enterprise.h"
#include "main.h"
#include "memory.h"
#include "main-private.h"

int execute_enterprise(int enterp_id, struct comm_buffers* buffers, struct main_data* data) {
    // first, setup op and op_counter
    struct operation op = {0, 0, 0, 0, 0, 0};
    int op_counter = 0;
    while ((*data->terminate) == 0) { // while did not receive terminate flag...
        enterprise_receive_operation(&op, enterp_id, buffers, data); // read op from buffer
        // if there's no op to do
        if (op.id == -1) 
            continue; // skip this iteration
        else
            printf(INFO_RECEIVED_OP, "Enterprise", enterp_id, op.id);
        
        // found operation to do! processing it...
        enterprise_process_operation(&op, enterp_id, data, &op_counter);

    }
    // return counter of processed operations
    return op_counter;
}

void enterprise_receive_operation(struct operation* op, int enterp_id, struct comm_buffers* buffers, struct main_data* data) {
    if (*(data->terminate) == 1) return; // return if program received terminate flag
    // read interm-enterprise buffer
    read_interm_enterp_buffer(buffers->interm_enterp, enterp_id, data->buffers_size, op);
}

void enterprise_process_operation(struct operation* op, int enterp_id, struct main_data* data, int* counter) {
    op->receiving_enterp = enterp_id; // update receiving enterp field
    // change status to 'A' or 'E' (processed by enterp)
    op->status = (op->id < data->max_ops) ? 'E' : 'A' ; 
    (*counter)++; // increment counter of this enterprise's processed operations
    memcpy(&data->results[op->id], op, sizeof(struct operation));
}