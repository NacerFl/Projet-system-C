FROM alpine:latest
RUN apk update
RUN apk add gcc libc-dev gdb vim
COPY shell.c /home/projet/shell.c
WORKDIR /home/projet/
RUN gcc -o jobshell shell.c 

CMD [ "./jobshell" ]