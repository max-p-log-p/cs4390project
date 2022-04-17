#include "proc.h"

enum Args { PROG_NAME = 0, DESTINATION, PORT, ARGS_LEN };

#define MAX_SLEEP 2000
#define USAGE_STR "cl destination port"
