all: makec

makec:
	gcc -Wall "shell.c" -o "jobshell"


reset:
	rm -f "jobshell"