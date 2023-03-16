#include <stdlib.h>
#include "../include/enterprise.h"
#include "../include/main.h"
#include "../include/memory.h"

int execute_enterprise(int enterp_id, struct comm_buffers* buffers, struct main_data* data) {
    // first, setup op and op_counter
    struct operation* op;
    int op_counter = 0;
    while (data->terminate == 0) { // while did not receive terminate flag...
        op = NULL; // every new iteration, op should be set to NULL
        enterprise_receive_operation(op, enterp_id, buffers, data); // read op from buffer
        if (op == NULL || op->id == -1) // if there's no op to do or get operation did not succeed
            continue; // skip this iteration
        // found operation to do! processing it...
        enterprise_process_operation(op, enterp_id, data, &op_counter);

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
    // TODO: validate which condition we should apply here
    op->status = ((*counter) < data->max_ops) ? 'E' : 'A' ; 
    (*counter)++; // increment counter of this enterprise's processed operations
    // TODO: what do you mean by "Atualiza também a operação na estrutura data"?
}
