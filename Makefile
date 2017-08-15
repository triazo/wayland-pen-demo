CC="/usr/bin/clang"

all:
	$(CC) *.c -o draw `pkg-config --cflags --libs gtk+-3.0`
