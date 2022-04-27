#include <err.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "math.h"
#include "net.h"
#include "cl.h"

int32_t
main(int32_t argc, char * const *argv)
{
	int32_t i, sfd;
	struct Msg msg;

	if (argc != ARGS_LEN)
		usage(USAGE_STR);

	/* when tloc is NULL, time() can't fail */
	srand(time(NULL));

	if ((sfd = createSocket(argv[DESTINATION], argv[PORT], 0)) < 0)
		err(1, "createSocket");

	for (i = 0; i < MIN_REQS + rand(); ++i) {
		usleep(rand() % MAX_SLEEP);

		msg.op = rand() % NUM_OPS;
		msg.arg1 = rand() % (2 << CHAR_BIT);
		msg.arg2 = rand() % (2 << CHAR_BIT);

		printMsg("request:", msg, 0);

		if (writeMsg(sfd, msg))
			err(1, "writeMsg");

		if (readMsg(sfd, &msg))
			err(1, "readMsg");

		printMsg("reply:", msg, 1);
	}

	close(sfd);

	return 0;
}
