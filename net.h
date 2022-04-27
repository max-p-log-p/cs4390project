#include <stdint.h>

struct Msg {
	uint8_t op;
	uint8_t arg1;
	uint8_t arg2;
	int32_t result;
} __attribute__((packed));

extern int32_t readMsg(int32_t, struct Msg *);
extern int32_t writeMsg(int32_t, struct Msg);
extern int32_t createSocket(const char *, const char *, int32_t);
extern int32_t listenSocket(const char *, const char *);
extern int32_t acceptSocket(int32_t, char *, size_t);
extern void printMsg(const char *, struct Msg, int8_t showResult);
