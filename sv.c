#include <err.h>
#include <pthread.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "math.h"
#include "net.h"
#include "sv.h"

void *reply(void *);

int32_t
main(int32_t argc, char * const *argv)
{
	uint64_t i;
	pthread_t repliers[NUM_CLIENTS];

	if (argc != ARGS_LEN)
		usage(USAGE_STR);

	/* NULL is 0.0.0.0 */
	if ((lfd = listenSocket(NULL, argv[PORT])) < 0)
		err(1, "listenSocket");

	for (i = 0; i < LEN(repliers); ++i) {
		if (pthread_create(&repliers[i], NULL, reply, NULL))
			err(1, "pthread_create");
	}

	/* wait for threads */
	for (i = 0; i < LEN(repliers); ++i) {
		if (pthread_join(repliers[i], NULL))
			warn("pthread_join");
	}

	close(lfd);

	return 0;
}

void *
reply(void *)
{
	int32_t afd;
	uint64_t i;
	struct Msg msg;

	if ((afd = acceptSocket(lfd)) < 0)
		err(1, "accept");

	for (i = 0; i < NUM_REQS; ++i) {
		if (readMsg(afd, &msg))
			err(1, "recv");

		printMsg("msg:", msg);

		switch (msg.op) {
		case ADD:
			msg.result = msg.arg1 + msg.arg2;
			break;
		case SUB:
			msg.result = msg.arg1 - msg.arg2;
			break;
		case MUL:
			msg.result = msg.arg1 * msg.arg2;
			break;
		case DIV:
			if (msg.arg2 != 0)
				msg.result = msg.arg1 / msg.arg2;
			else
				msg.op = ERR;
			break;
		default:
			msg.op = ERR;
			break;
		}

		if (writeMsg(afd, msg))
			err(1, "writeMsg");
	}

	close(afd);
	return NULL;
}
