#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "user/user.h"

char *
fmtname(char *path)
{
  static char buf[DIRSIZ + 1];
  char *p;
  for (p = path + strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  memmove(buf, p, strlen(p));
  buf[strlen(p)] = '\0';
  return buf;
}

void
find(char *path, char *target, char *cmd, char *argv[], int cur)
{
  char *p;
  char buf[512];
  int fd;
  struct dirent de;
  struct stat st;

  if ((fd = open(path, O_RDONLY)) < 0) {
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }

  if (fstat(fd, &st) < 0) {
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }

  if (strcmp(fmtname(path), target) == 0) {
    if (cur) {
      int pid = fork();
      if (pid == 0) {
        argv[cur++] = path;
        argv[cur] = 0;
        exec(cmd, argv);
        fprintf(2, "find: exec %s failed\n", cmd);
        exit(1);
      }
      wait(0);
    } else {
      printf("%s\n", path);
    }
  }

  if (st.type == T_DIR) {
    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/';
    while (read(fd, &de, sizeof(de)) == sizeof(de)) {
      if (de.inum == 0 || strcmp(de.name, ".") == 0 ||
          strcmp(de.name, "..") == 0) {
        continue;
      }
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = '\0';
      find(buf, target, cmd, argv, cur);
    }
  }
  close(fd);
}

int
main(int argc, char **argv)
{
  if (argc == 3) {
    find(argv[1], argv[2], 0, 0, 0);
    exit(0);
  }

  if (argc >= 5 && strcmp(argv[3], "-exec") == 0) {
    int i;
    char **execcmd = malloc((argc - 2) * sizeof(char *));
    int cur = 0;
    for (i = 4; i < argc; i++) {
      execcmd[cur++] = argv[i];
    }

    find(argv[1], argv[2], argv[4], execcmd, cur);
    exit(0);
  }

  fprintf(2, "Usage: find path target [-exec cmd args]\n");
  exit(1);
}