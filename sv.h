#include "proc.h"

#define USAGE_STR "sv destination port"
#define MAX_QUEUE 4096

struct Entry {
	struct Msg msg;
	int32_t sfd;
};

enum Mutexes { COND, EMPTY, FULL, HEAD, TAIL, NUM_MUTEXES };
enum Conds { _EMPTY, _FULL, NUM_CONDS };

static int32_t lfd;
static uint32_t head, tail;
static struct Entry fifo[MAX_QUEUE];

static pthread_mutex_t mutexes[NUM_MUTEXES];
static pthread_cond_t conds[NUM_CONDS];
