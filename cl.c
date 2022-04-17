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
	struct Msg req;

	if (argc != ARGS_LEN)
		usage(USAGE_STR);

	/* when tloc is NULL, time() can't fail */
	srand(time(NULL));

	if ((sfd = createSocket(argv[DESTINATION], argv[PORT], 0)) < 0)
		err(1, "createSocket");

	for (i = 0; i < 3; ++i) {
		usleep(rand() % MAX_SLEEP);

		req.op = rand() % NUM_OPS;
		req.arg1 = rand() % (2 << CHAR_BIT);
		req.arg2 = rand() % (2 << CHAR_BIT);

		printMsg("req:", req);

		if (writeMsg(sfd, req))
			err(1, "writeMsg");
	}

	close(sfd);

	return 0;
}
