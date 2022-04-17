#define NUM_SERVERS 1
#define NUM_CLIENTS 2
#define MIN_REQS 3
#define LEN(array) (sizeof(array) / sizeof(array[0]))

enum Args { PROG_NAME, DESTINATION, PORT, ARGS_LEN };

void usage(const char *);
