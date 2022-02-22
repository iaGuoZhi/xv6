#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char **argv) {
    int pid;
    int ping_fds[2], pong_fds[2];
    int buf, val = 1;

    if(pipe(ping_fds) < 0 || pipe(pong_fds) < 0){
        printf("pipe() failed\n");
        exit(1);
    }
    pid = fork();
    if(pid == 0){
        pid = getpid();
        if(read(ping_fds[0], &buf, 4))
        {
            printf("%d: received ping\n", pid);
        }
        write(pong_fds[1], &val, 4); 
    }else{
        pid = getpid();
        write(ping_fds[1], &val, 4); 
        if(read(pong_fds[0], &buf, 4))
        {
            printf("%d: received pong\n", pid);
        }
    }
    exit(0);
}
