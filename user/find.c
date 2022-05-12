#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

void find(char *path, char *pattern)
{
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    if((fd = open(path, 0)) < 0){
        fprintf(2, "ls: cannot open %s\n", path);
        return;
    }

    if(fstat(fd, &st) < 0){
        fprintf(2, "ls: cannot stat %s\n", path);
        close(fd);
        return;
    }

    if(st.type == T_DIR){
        strcpy(buf, path);
        p = buf+strlen(buf);
        *(p++) = '/'; 

        while(read(fd, &de, sizeof(de)) == sizeof(de)){
            if(de.inum == 0)
                continue;
            if(strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
                continue;
            memmove(p, de.name, DIRSIZ);
            p[DIRSIZ] = 0;
            if(stat(buf, &st) < 0){
                printf("find: cannot stat %s\n", buf);
                continue;
            }
            if(strcmp(de.name, pattern) == 0){
                printf("%s\n", buf);
            }
            if(st.type == T_DIR){
                find(buf, pattern);
            }
        }
    }
    close(fd);
}

int main(int argc, char *argv[])
{
    char *path, *pattern;

    if(argc < 3){
        fprintf(2, "usage: find pattern [path ... pattern ...]\n");
        exit(1);
    }

    path = argv[1];
    pattern = argv[2];
    find(path, pattern);
    exit(0);
}
