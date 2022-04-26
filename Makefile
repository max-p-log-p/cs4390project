CC = cc
CFLAGS = -Wall -Wextra -Werror -O2

all: sv cl run

run:
	gnome-terminal -e " ./sv 127.0.0.1"; ./cl 127.0.0.1 22; ./cl 127.0.0.1 22

cl: cl.o net.o proc.o
	$(CC) $(LDFLAGS) -o cl cl.o net.o proc.o

sv: sv.o net.o proc.o
	$(CC) $(LDFLAGS) -pthread -o sv sv.o net.o proc.o

sv.o: sv.c net.h sv.h proc.h
cl.o: cl.c net.h cl.h proc.h
net.o: net.c net.h
proc.o: proc.c proc.h

clean:
	rm -f sv cl net.o sv.o cl.o proc.o
