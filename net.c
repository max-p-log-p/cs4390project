#include <arpa/inet.h>

#include <err.h>
#include <errno.h>
#include <netdb.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "math.h"
#include "net.h"
#include "proc.h"

int32_t
readMsg(int32_t sockfd, struct Msg *msg)
{
	int64_t nr;
	uint64_t i;
	uint8_t buf[sizeof(struct Msg)];
	struct Msg *recvd;

	for (i = 0; i < sizeof(buf); i += nr) {
		if ((nr = read(sockfd, buf + i, sizeof(buf) - i)) > 0)
			continue;

		if (nr == 0)
			return -1;

		switch (errno) {
		case EINTR:
			nr = 0;
			continue;
		default:
			return -1;
		}
	}

	recvd = (struct Msg *)buf;
	msg->op = recvd->op;
	msg->arg1 = recvd->arg1;
	msg->arg2 = recvd->arg2;
	msg->result = recvd->result;
	return 0;
}

int32_t
writeMsg(int sockfd, struct Msg msg)
{
	return write(sockfd, &msg, sizeof(msg)) != sizeof(msg);
}

/* returns socket file descriptor */
int
createSocket(const char *node, const char *service, int32_t lflag)
{
	int ecode, optval, serrno, sockfd;
	struct addrinfo *cur, hints, *res;

	memset(&hints, 0, sizeof(struct addrinfo));

	if (lflag) {
		hints.ai_flags = AI_PASSIVE;
		/* default to more common ipv4 if wildcard address */
		hints.ai_family = (node == NULL ? AF_INET : AF_UNSPEC);
	} else
		hints.ai_family = AF_UNSPEC;

	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	if ((ecode = getaddrinfo(node, service, &hints, &res)))
		errx(1, "getaddrinfo: %s", gai_strerror(ecode));

	for (cur = res; cur; cur = cur->ai_next) {
		if ((sockfd = socket(cur->ai_family, cur->ai_socktype,
		    cur->ai_protocol)) == -1)
			continue;

		optval = 1;

		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &optval, 
		    sizeof(optval)))
			err(1, "setsockopt");

		if (lflag) {
			if (bind(sockfd, cur->ai_addr, cur->ai_addrlen) == 0)
				break;
		} else {
			if (connect(sockfd, cur->ai_addr, cur->ai_addrlen) == 0)
				break;
		}

		serrno = errno; /* errno can be modified by close */
		if (close(sockfd))
			warn("can't close socket %d", sockfd);
		errno = serrno;
		sockfd = -1;
	}

	freeaddrinfo(res);
	return sockfd;
}

int32_t
listenSocket(const char *node, const char *service)
{
	int32_t sockfd;

	if ((sockfd = createSocket(node, service, 1)) < 0)
		err(1, "createSocket");

	if (listen(sockfd, SOMAXCONN))
		err(1, "listen");

	return sockfd;
}

/* return the accepted file descriptor */
int32_t
acceptSocket(int32_t sockfd, char *dst, size_t size)
{
	int32_t afd;
	socklen_t addrlen;
	struct sockaddr_storage addr;
	struct sockaddr_in *src;
	struct sockaddr_in6 *src6;

	addrlen = sizeof(addr);

	while ((afd = accept(sockfd, (struct sockaddr *)&addr, &addrlen)) < 0) {
		switch (errno) {
		case ECONNABORTED:
		case EINTR:
			warn("accept failed");
			break;
		default:
			return -1;
		}
	}

	switch(addr.ss_family) {
	case AF_INET:
		src = (struct sockaddr_in *)&addr;
		if (inet_ntop(addr.ss_family, (void *)&src->sin_addr, dst, size) == NULL)
			err(1, "inet_ntop");
		break;
	case AF_INET6:
		src6 = (struct sockaddr_in6 *)&addr;
		if (inet_ntop(addr.ss_family, (void *)&src6->sin6_addr, dst, size) == NULL)
			err(1, "inet_ntop");
		break;
	default:
		return -1;
	}

	return afd;
}

void
printMsg(const char *str, struct Msg msg)
{
	if(msg.op < LEN(OP_CHARS))
	{
		if (strcmp(str,REQUEST_STR)) //if they do not match, then print the result along with the calculation, otherwise print a question mark where the result should be
			printf("%s %d %c %d = %d\n", str, msg.arg1, OP_CHARS[msg.op], msg.arg2, msg.result);
		else
			printf("%s %d %c %d = ?\n", str, msg.arg1, OP_CHARS[msg.op], msg.arg2);
	}
	else
		printf(" %s %d %x %d = ?\n", str, msg.arg1, msg.op, msg.arg2);
}
