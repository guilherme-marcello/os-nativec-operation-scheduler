/* Função que verifica se a condicao condition se verifica e,
se for o caso, lanca uma excecao com a mensagem error_msg. 
Indica na excecao o "snippet_id" associado ao erro.
*/
void verify_condition(int condition, char* snippet_id, char* error_msg);

/* Funcao que devolve o menor inteiro passado como argumento. */
int min(int a, int b);

#define NUMBER_OF_ARGS 6
#define FLAGS_OPTIONS "help menu: -h\n"
#define USAGE_STR "./AdmPor max_ops buffers_size n_clients n_intermediaries n_enterprises\n"
#define PROGRAM_NAME "AdmPor\n"


// Error handling constants
#define ERROR_ARGS "Error: Number of arguments is should be 6. Use -h flag for help.\n"
#define ERROR_MALLOC "Error: failed to allocate memory.\n"

#define INIT_DS "Init DS"
#define INIT_COMM_BUFFER "Init comm buffer"
#define INIT_MAIN "Init main Args"
#define INIT_DMEM_BUFFERS "Init dmem buffers"
#define INIT_SHMEM_BUFFERS "Init shmem buffers"
