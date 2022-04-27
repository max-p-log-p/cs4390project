#!/bin/sh

if [ ! -f "sv" -o ! -f "cl" ]; then
	echo "Run make to compile the program."
fi

killall -e sv cl 2>/dev/null

(sleep 1; ./cl 127.0.0.1 1024) &
(sleep 1; ./cl 127.0.0.1 1024) &
./sv 127.0.0.1 1024
