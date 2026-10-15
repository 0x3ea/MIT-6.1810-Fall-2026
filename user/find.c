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
find(char *path, char *target)
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
    printf("%s\n", path);
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
      find(buf, target);
    }
  }
  close(fd);
}

int
main(int argc, char **argv)
{
  if (argc != 3) {
    fprintf(2, "Usage: find path target\n");
    exit(1);
  }

  find(argv[1], argv[2]);
  exit(0);
}