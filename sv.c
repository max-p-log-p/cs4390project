#include <arpa/inet.h>

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

void *reply();
void *queue();
void pthreads_create(pthread_t *, void *(*)(void *), size_t);
void pthreads_join(pthread_t *, size_t);

void
pthreads_create(pthread_t *threads, void *(*start_routine)(void *), size_t len)
{
	uint64_t i;
	for (i = 0; i < len; ++i) {
		if (pthread_create(&threads[i], NULL, start_routine, NULL))
			err(1, "pthread_create");
	}
}

void
pthreads_join(pthread_t *threads, size_t len)
{
	uint64_t i;
	for (i = 0; i < len; ++i) {
		if (pthread_join(threads[i], NULL))
			warn("pthread_join");
	}
}

int32_t
main(int32_t argc, char * const *argv)
{
	pthread_t queuers[NUM_CLIENTS], repliers[NUM_CLIENTS];

	if (argc != ARGS_LEN)
		usage(USAGE_STR);

	/* NULL is 0.0.0.0 */
	if ((lfd = listenSocket(NULL, argv[PORT])) < 0)
		err(1, "listenSocket");

	pthreads_create(queuers, queue, sizeof(queuers));
	pthreads_create(repliers, reply, sizeof(repliers));

	/* wait for threads */
	pthreads_join(queuers, sizeof(queuers));
	pthreads_join(repliers, sizeof(repliers));

	close(lfd);

	return 0;
}

void *
queue(void *args __attribute__((unused)) )
{
	int32_t afd;
	struct Msg msg;
#if INET6_ADDRSTRLEN > INET_ADDRSTRLEN
	char dst[INET6_ADDRSTRLEN];
#else
	char dst[INET_ADDRSTRLEN];
#endif

	for (;;) {
		if ((afd = acceptSocket(lfd, dst, sizeof(dst))) < 0)
			err(1, "accept");

		printf("%s %ld 0\n", dst, time(NULL));

		for (;;) {
			if (readMsg(afd, &msg))
				break;

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

			printMsg(dst, msg, 1);

			/* block if full */
			pthread_mutex_lock(&mutexes[FULL]);
			while ((tail + 1) % LEN(fifo) == head)
				pthread_cond_wait(&conds[_FULL], &mutexes[FULL]);
			pthread_mutex_unlock(&mutexes[FULL]);

			/* add to circular queue */
			pthread_mutex_lock(&mutexes[TAIL]);
			fifo[tail].sfd = afd;
			fifo[tail].msg = msg;
			tail = (tail + 1) % LEN(fifo);
			pthread_mutex_unlock(&mutexes[TAIL]);

			pthread_cond_broadcast(&conds[_EMPTY]);
		}

		printf("%s %ld 1\n", dst, time(NULL));

		close(afd);
	}
	return NULL;
}

void *
reply(void *args __attribute__((unused)))
{
	for (;;) {
		/* block if empty */
		pthread_mutex_lock(&mutexes[EMPTY]);
		while (head == tail)
			pthread_cond_wait(&conds[_EMPTY], &mutexes[EMPTY]);
		pthread_mutex_unlock(&mutexes[EMPTY]);

		writeMsg(fifo[head].sfd, fifo[head].msg);

		pthread_mutex_lock(&mutexes[HEAD]);
		head = (head + 1) % LEN(fifo);
		pthread_mutex_unlock(&mutexes[HEAD]);

		pthread_cond_broadcast(&conds[_FULL]);
	}
	return NULL;
}
