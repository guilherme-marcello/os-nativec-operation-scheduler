/**
 * SO-035
 * Guilherme Marcelo    <fc58173>
 * Eduardo Santos       <fc58185>
 * Xi Wang              <fc58183>
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "main.h"
#include "main-private.h"
#include "memory.h"
#include "process.h"

// global counter of created operations
int operation_number = 0;

void verify_condition(int condition, char* snippet_id, char* error_msg, int status) {
    if (condition) {
        fprintf(stderr, "[%s] %s", snippet_id, error_msg);
        exit(status);        
    }
}

void usage_menu(int argc, char** argv) {
    if (argc == 2 && strcmp(argv[1], "-h") == 0) {
        // print usage string
        printf(USAGE_STR);
        // exit program
        exit(EXIT_SUCCESS);
    }
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
        data->buffers_size * sizeof(int)
    );

    buffers->main_client->buffer = create_shared_memory(
        STR_SHM_MAIN_CLIENT_BUFFER, 
        data->buffers_size * sizeof(struct operation)
    );

    buffers->client_interm->ptrs = create_shared_memory(
        STR_SHM_CLIENT_INTERM_PTR, 
        sizeof(struct pointers)
    );

    buffers->client_interm->buffer = create_shared_memory(
        STR_SHM_CLIENT_INTERM_BUFFER, 
        data->buffers_size * sizeof(struct operation)
    );

    buffers->interm_enterp->ptrs = create_shared_memory(
        STR_SHM_INTERM_ENTERP_PTR, 
        data->buffers_size * sizeof(int)
    );

    buffers->interm_enterp->buffer = create_shared_memory(
        STR_SHM_INTERM_ENTERP_BUFFER, 
        data->buffers_size * sizeof(struct operation)
    );

    // main_data-related fields
    data->results = create_shared_memory(
        STR_SHM_RESULTS, 
        sizeof(struct operation) * MAX_RESULTS
    ); 
    data->terminate = create_shared_memory(
        STR_SHM_TERMINATE, 
        sizeof(int)
    );
}

void launch_process(struct comm_buffers* buffers, struct main_data* data, int* pids, int n, LaunchFunc launch_func) {
    // for each process to launch, use 0 as "root id" and save its pid
    for (int i = 0; i < n; i++)
        pids[i] = launch_func(i, buffers, data);
}

void flush() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}

void create_request(int* op_counter, struct comm_buffers* buffers, struct main_data* data) {
    int client_id, enterprise_id;
    // read client id and enterprise id
    if (scanf("%d %d", &client_id, &enterprise_id) != 2) {
        printf(ERROR_INVALID_INPUT);
        flush();
        return;
    }

    // verify if given ids are valid
    if (client_id < 0 || client_id >= data->n_clients || enterprise_id < 0 || enterprise_id >= data->n_enterprises) {
        printf(ERROR_IDS_OUT_OF_BOUNDS);
        return;
    }

    // create operation, update data->results and update operation fields
    struct operation op = {0, 0, 0, 0, 0};
    data->results[*op_counter] = op;
    op.id = *op_counter;
    op.requesting_client = client_id;
    op.requested_enterp = enterprise_id;
    op.status = 'M';

    // print successfull message, write operation in the main-client buffer and increment operation counter
    printf(INFO_CREATED_OP, "Operation request", op.id);
    write_main_client_buffer(buffers->main_client, data->buffers_size, &op);   
    (*op_counter)++; 
}

int convert_status_to_int(char status) {
    //Associate each status with a number
    int statusInt = 0;
    if (status == 'C')
        statusInt = 1;
    else if (status == 'I')
        statusInt = 2;
    else if (status == 'A')
        statusInt = 3;
    else if (status == 'E')
        statusInt = 4;   
    return statusInt;
}

void read_status(struct main_data* data) {
    // read operation id
    int operation_id;
    if (scanf("%d", &operation_id) != 1) {
        printf(ERROR_INVALID_INPUT);
        flush();
        return;
    }

    // verify if given id is valid
    if (operation_id < 0 || operation_id >= operation_number) {
        printf(ERROR_OPERATION_ID);
        return;
    }

    // read operation from data->results for the given id
    struct operation op = data->results[operation_id];
    // convert status to an integer
    int statusInt = convert_status_to_int(op.status);
    // use it to compose an appropriate status message
    printf(INFO_STATUS_OPERATION, op.id, op.status);
    printf(INFO_STATUS_MAIN_CREATED, op.requesting_client, op.requested_enterp);
    if (statusInt >= 1)
        printf(INFO_STATUS_CLIENT_PROCESSED, op.receiving_client);
    if (statusInt >= 2)
        printf(INFO_STATUS_INTERM_PROCESSED, op.receiving_interm);
    if (statusInt == 3)
        printf(INFO_STATUS_ENTERP_BOOKED, op.receiving_enterp);
    if (statusInt >= 4)
        printf(INFO_STATUS_ENTERP_PROCESSED, op.receiving_enterp);
}

void help() {
    printf("%s\n", HELP_MSG);
}

void write_statistic(char* entity, int n, int* stats) {
    char rows[1024] = "";

    for (int i = 0; i < n; i++) {
        // format the string and write it to a temporary buffer
        char temp[1024];
        snprintf(temp, 1024, "   | %-15d| %25d |\n", i, stats[i]);
        // append the temporary buffer to the main buffer
        strcat(rows, temp);
    }

    printf(STATISTIC_MSG, entity, rows);
}

void write_statistics(struct main_data* data) {
    printf("====================\033[0;32m Statistics\033[0m ====================\n");
    printf(" Max number of operations: %24d\n",     data->max_ops);
    printf(" Buffer size: %37d\n",                  data->buffers_size);

    printf("\n Number of clients: %31d\n",            data->n_clients);
    write_statistic("Client  ", data->n_clients, data->client_stats);
    
    printf("\n Number of intermediaries: %24d\n",     data->n_intermediaries);
    write_statistic("Interm. ", data->n_intermediaries, data->intermediary_stats);
    
    
    printf("\n Number of enterprises: %27d\n",        data->n_enterprises);
    write_statistic("Enterpr.", data->n_enterprises, data->enterprise_stats);

}

void wait_processes(struct main_data* data) {
    for (int i = 0; i < data->n_clients; i++)
        data->client_stats[i] = wait_process(data->client_pids[i]);
    for (int i = 0; i < data->n_intermediaries; i++)
        data->intermediary_stats[i] = wait_process(data->intermediary_pids[i]);
    for (int i = 0; i < data->n_enterprises; i++)
        data->enterprise_stats[i] = wait_process(data->enterprise_pids[i]);
}

void cleanup_memory(struct comm_buffers* buffers, struct main_data* data) {
    // first shared memory
    destroy_shared_memory(STR_SHM_MAIN_CLIENT_PTR, buffers->main_client->ptrs, data->buffers_size);
    destroy_shared_memory(STR_SHM_MAIN_CLIENT_BUFFER, buffers->main_client->buffer, data->buffers_size);
    destroy_shared_memory(STR_SHM_CLIENT_INTERM_PTR, buffers->client_interm->ptrs, data->buffers_size);
    destroy_shared_memory(STR_SHM_CLIENT_INTERM_BUFFER, buffers->client_interm->buffer, data->buffers_size);
    destroy_shared_memory(STR_SHM_INTERM_ENTERP_BUFFER, buffers->interm_enterp->buffer, data->buffers_size);
    destroy_shared_memory(STR_SHM_INTERM_ENTERP_PTR, buffers->interm_enterp->ptrs, data->buffers_size);
    destroy_shared_memory(STR_SHM_RESULTS, data->results, data->buffers_size);
    destroy_shared_memory(STR_SHM_TERMINATE, data->terminate, data->buffers_size);

    // then dynamic memory
    destroy_dynamic_memory(data);
    destroy_dynamic_memory(buffers->main_client);
    destroy_dynamic_memory(buffers->client_interm);
    destroy_dynamic_memory(buffers->interm_enterp);
    destroy_dynamic_memory(buffers);
}

void stop_execution(struct main_data* data, struct comm_buffers* buffers) {
    *(data->terminate) = 1;
    wait_processes(data);
    write_statistics(data);
    cleanup_memory(buffers, data);
}

void user_interaction(struct comm_buffers *buffers, struct main_data *data) {
    int running = TRUE;
    char input[32]; // user input buffer

    while (running) {
        // sleep 300000 micro sec and launch shell 
        usleep(300000);
        printf(ADMPOR_SHELL);

        // ask for input
        if (scanf("%s", &input) != 1) {
            printf(ERROR_INVALID_INPUT);
            flush();
            return;
        }
        if (strcmp(input, "op") == 0) {
            create_request(&operation_number, buffers, data);
        } else if (strcmp(input, "status") == 0) {
            read_status(data);
        } else if (strcmp(input, "stop") == 0) {
            running = FALSE;
            stop_execution(data, buffers);
        } else if (strcmp(input, "help") == 0) {
            help();
        } else {
            printf(ERROR_UNRECOGNIZED_COMMAND);
            flush();
        }
    }
}

void launch_processes(struct comm_buffers* buffers, struct main_data* data) {
    launch_process(buffers, data, data->client_pids, data->n_clients, launch_client);
    launch_process(buffers, data, data->intermediary_pids, data->n_intermediaries, launch_interm);
    launch_process(buffers, data, data->enterprise_pids, data->n_enterprises, launch_enterp);
}

int main(int argc, char *argv[]) {
    //init data structures
    struct main_data* data = create_dynamic_memory(sizeof(struct main_data));
    struct comm_buffers* buffers = create_dynamic_memory(sizeof(struct comm_buffers));
    verify_condition(!data || !buffers, INIT_DS , ERROR_MALLOC, EXIT_FAILURE);

    buffers->main_client = create_dynamic_memory(sizeof(struct rnd_access_buffer));
    buffers->client_interm = create_dynamic_memory(sizeof(struct circular_buffer));
    buffers->interm_enterp = create_dynamic_memory(sizeof(struct rnd_access_buffer));
    verify_condition(
        !buffers->main_client || !buffers->client_interm || !buffers->interm_enterp, 
        INIT_COMM_BUFFER,
        ERROR_MALLOC,
        EXIT_FAILURE
    );

    // launch usage menu
    usage_menu(argc, argv);
    verify_condition(
        argc != NUMBER_OF_ARGS,
        INIT_MAIN,
        ERROR_ARGS,
        EXIT_FAILURE
    );
    //execute main code
    main_args(argc, argv, data);
    
    create_dynamic_memory_buffers(data);
    verify_condition(
        !data->client_pids || !data->intermediary_pids || !data->enterprise_pids 
        || !data->client_stats || !data->intermediary_stats || !data->enterprise_stats, 
        INIT_DMEM_BUFFERS,
        ERROR_MALLOC,
        EXIT_FAILURE
    );
    
    create_shared_memory_buffers(data, buffers);
    verify_condition(
        !data->results || !data->terminate 
        || !buffers->main_client->ptrs || !buffers->main_client->buffer
        || !buffers->client_interm->ptrs || !buffers->client_interm->buffer
        || !buffers->interm_enterp->ptrs || !buffers->interm_enterp->buffer, 
        INIT_SHMEM_BUFFERS,
        ERROR_MALLOC,
        EXIT_FAILURE
    );

    // launch clients, interms and enterps
    launch_processes(buffers, data);

    printf(MENU_MSG);
    // launch user interaction menu
    user_interaction(buffers, data);
}
