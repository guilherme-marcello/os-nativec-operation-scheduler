#ifndef MEMORY_PRIVATE_H_GUARD
#define MEMORY_PRIVATE_H_GUARD

#define FREE_MEM 0
#define USED_MEM 1

// Error handling constants
#define ERROR_SHM_OPEN "Error: Failed to open shared memory region.\n"
#define ERROR_SHM_TRUNCATE "Error: Failed to truncate shared memory region.\n"
#define ERROR_SHM_MAP "Error: Failed to map shared memory region.\n"
#define ERROR_SHM_UNMAP "Error: Failed to unmap shared memory region.\n"
#define ERROR_SHM_UNLINK "Error: Failed to unlink shared memory region.\n"

#define EXIT_MUNMAP_ERROR 7
#define EXIT_SHM_UNLINK_ERROR 8

#endif
