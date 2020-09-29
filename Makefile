
build:
	gcc -o simple_shell simple_shell.c utils.c utils.h

debug:
	gdb ./simple_shell

run:
	./simple_shell
