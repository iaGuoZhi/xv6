#include "kernel/types.h"
#include "kernel/param.h"
#include "user/user.h"

int append_argv(char **exec_argv, int *argv_idx, char *buf)
{
    char c;
    int i = 0;

    if(read(0, &c, sizeof(char)) == 0)
        return 0;

    if(c == '\0')
        return 0;

    while(c != '\n'){
        if(c != ' '){
            buf[i] = c;
            if(i == 0 || buf[i - 1] == ' '){
                exec_argv[(*argv_idx)] = &(buf[i]);
                *argv_idx = *argv_idx + 1;
            }
            i += 1;
        }
        if(read(0, &c, sizeof(char)) == 0)
            return 0;
    }
    buf[i] = '\0';

    return 1;
}


int main(int argc, char **argv) {
    int argv_idx = 0, t;
    int pid, xstatus;
    int i;
    char *exec_argv[MAXARG]; 
    char buf[512]; 

    for(i = 1; i < argc; i++){
        exec_argv[argv_idx++] = argv[i];
    }

    while(1){
        t = argv_idx;
        if(!append_argv(exec_argv, &t, buf)){
            break;
        }
        
        // fork + exec
        pid = fork();
        if(pid < 0){
            printf("fork failed\n");
            exit(1);
        }
        if(pid == 0){
            if(exec(exec_argv[0], &(exec_argv[0])) < 0){
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
