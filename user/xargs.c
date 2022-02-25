#include "kernel/types.h"
#include "kernel/param.h"
#include "user/user.h"

int main(int argc, char **argv) {
    int argv_idx = 0, t;
    int pid, xstatus;
    int i;
    char *exec_argv[MAXARG]; 
    char buf[512], c = ' ';

    for(i = 1; i < argc; i++){
        exec_argv[argv_idx++] = argv[i];
    }

    while(c != '\0'){
        i = -1;
        t = argv_idx;
        while(c != '\0'){
            read(0, &c, sizeof(char));
            if(c == '\0')
                break;
            printf("c: %c\n", c);
            if(i >= 0 && buf[i] != '\0'){
                if(c != ' '){
                    buf[++i] = c;
                }else{
                    buf[++i] = '\0';
                }
            }else{
                if(c != ' '){
                    buf[++i] = c;
                    exec_argv[t++] = &(buf[i]);
                }
            }
        } 
        if(i == -1)
            break;
        buf[i] = '\0';
        for(i = 0; i < t; ++i){
            printf("argv[%d]: %s\n", i,  exec_argv[i]);
        }
        
        // fork + exec
        pid = fork();
        if(pid < 0){
            printf("fork failed\n");
            exit(1);
        }
        if(pid == 0){
            if(exec(exec_argv[0], &(exec_argv[1])) < 0){
                printf("exec failed\n");
                exit(1);
            }
        }else{
            if(wait(&xstatus) != pid){
                printf("wait failed\n");
            }
        }
    }
    exit(0);
}
