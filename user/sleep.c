#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char **argv) {
  if (argc <= 1) {
    fprintf(2, "usage: sleep pattern [time ...]\n");
    exit(1);
  }

  if (argc <= 2) {
    sleep(atoi(argv[1]));
    exit(1);
  }

  exit(0);
}
