NAME=serial

CFLAG =-std=gnu99 -Os -Wall -pipe -D_GNU_SOURCE

MACHINE := $(shell uname -m)
ifeq ($(MACHINE), x86_64)
	LIB_DIR = /usr/lib64
else
	LIB_DIR = /usr/lib
endif

$(NAME): serial.o
	gcc -o $(NAME) serial.o -L$(LIB_DIR) -ldl
	strip $(NAME)
serial.o: serial.c
	gcc -c serial.c $(CFLAG)
clean:
	rm -f *.o $(NAME)


