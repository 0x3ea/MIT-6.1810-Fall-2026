#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"

void
settle(int hasnum, int bad, int num)
{
  if (hasnum && !bad && (num % 5 == 0 || num % 6 == 0))
    printf("%d\n", num);
}

int
issep(char c)
{
  return strchr(" -\r\t\n./,", c) != 0;
}

void
scan(int fd)
{
  char buf[512];
  int n, i;
  int num = 0;
  int hasnum = 0;
  int bad = 0;
  while ((n = read(fd, buf, sizeof(buf))) > 0) {
    for (i = 0; i < n; i++) {
      if (buf[i] >= '0' && buf[i] <= '9') {
        hasnum = 1;
        num = num * 10 + buf[i] - '0';
      } else if (issep(buf[i])) {
        settle(hasnum, bad, num);
        num = 0;
        hasnum = 0;
        bad = 0;
      } else {
        bad = 1;
      }
    }
  }
  settle(hasnum, bad, num);
}

int
main(int argc, char *argv[])
{
  if (argc == 1) {
    scan(0);
    exit(0);
  }

  int fd, i;
  for (i = 1; i < argc; i++) {
    fd = open(argv[i], O_RDONLY);
    if (fd < 0) {
      fprintf(2, "sixfive: cannot open %s\n", argv[i]);
      exit(1);
    }
    scan(fd);
    close(fd);
  }
  exit(0);
}
