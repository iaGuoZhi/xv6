#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

#define NUM_SIZE 34

void feed_numbers(int fds[2], int *nums, int *nums_len)
{
    int idx = 0;
    int val;

    while(read(fds[0], &val, 4)){
        nums[idx++] = val;
    }
    close(fds[0]);

    *nums_len = idx;
}

void put_numbers(int fds[2], int *nums, int *nums_len)
{
    int cur_prime;

    cur_prime = nums[0];
    printf("prime %d\n", cur_prime);

    for(int i = 1; i < *nums_len; ++i){
        if(nums[i] % cur_prime != 0)
        {
            // write num to pipe
            write(fds[1], &(nums[i]), 4);
        }
    }
    close(fds[1]);
}

int main(int argc, char *argv[])
{
    int nums[NUM_SIZE];
    int nums_len = NUM_SIZE;
    int pid, xstatus;
    int pipe_fds[2];

    for(int i = 0; i < NUM_SIZE; ++i){
        nums[i] = i + 2;
    }

    // only create process as needed
    while(nums_len >= 1){
        // create new pipe for new child process
        if(pipe(pipe_fds) < 0){
            printf("pipe() failed\n");
            exit(1);
        }
        pid = fork();
        // child
        if(pid == 0){
            close(pipe_fds[1]);
            feed_numbers(pipe_fds, nums, &nums_len);
        }
        // father
        else{
            close(pipe_fds[0]);
            put_numbers(pipe_fds, nums, &nums_len);
    
            if(wait(&xstatus) != pid){
                printf("wait failed\n");
            }
            // father process should not enter while loop
            break;
        }
    }
    exit(0);
}
