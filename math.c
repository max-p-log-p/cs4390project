#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>

#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <poll.h>
#include <string.h>

#include "math.h"

static int
read_socket(int sockfd, const char *buf, const char *pathname)
{
	int fd;
	char template[sizeof(TEMPLATE)] = TEMPLATE;
	ssize_t nr;
	size_t pos;
	unsigned char sbuf[BUFSIZE];

	pos = 0;

	// printf("read_socket: %d %s %s %d\n", sockfd, buf, pathname);

	while (pos < BUFSIZE) {
		if ((nr = read(sockfd, sbuf + pos, sizeof(sbuf) - pos)) <= 0)
			err(1, "recv");
		pos += nr;
	}

	return 0;
}

static int
write_socket(int sockfd, const char *buf, ssize_t count)
{
	ssize_t pos, nw;

	pos = 0;

	while (pos < BUFSIZE) {
		printf("write_socket: %ld\n", count);
		/* ensure sends are atomic */
		if ((nw = write(sockfd, buf + pos, count)) != count)
			err(1, "send");
		pos += nw;
	}

	// printf("write_socket: %d %s %ld\n", sockfd, buf, count);

	return 0;
}

/* returns socket file descriptor */
int
math_socket(const char *node, const char *service, int lflag)
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

void
math_listen(int sockfd)
{
	if (listen(sockfd, SOMAXCONN))
		err(1, "listen");
}

/* return the accepted file descriptor */
int
math_accept(int sockfd)
{
	int afd;
	socklen_t addrlen;
	struct sockaddr_storage addr;

	addrlen = sizeof(addr);

	if ((afd = accept(sockfd, (struct sockaddr *)&addr, &addrlen)) < 0)
		err(1, "accept");

	return afd;
}

int
math(const char *buf, int sockfd)
{
	read_socket(sockfd, buf);

	return fd;
}
