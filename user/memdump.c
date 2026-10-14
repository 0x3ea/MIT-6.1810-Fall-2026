#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"

void memdump(char *fmt, char *data, int len);

int
main(int argc, char *argv[])
{
  if (argc == 1) {
    printf("Example 1:\n");
    int a[2] = {61810, 2026};
    memdump("ii", (char *)a, sizeof(a));

    printf("Example 2:\n");
    memdump("S", "a string", sizeof("a string"));

    printf("Example 3:\n");
    char *s = "another";
    memdump("s", (char *)&s, sizeof(s));

    struct sss {
      char *ptr;
      int num1;
      short num2;
      char byte;
      char bytes[8];
    } example;

    example.ptr = "hello";
    example.num1 = 1819438967;
    example.num2 = 100;
    example.byte = 'z';
    strcpy(example.bytes, "xyzzy");

    printf("Example 4:\n");
    memdump("pihcS", (char *)&example, sizeof(example));

    printf("Example 5:\n");
    memdump("sccccc", (char *)&example, sizeof(example));
  } else if (argc == 2) {
    // format in argv[1], up to 512 bytes of data from standard input.
    char data[512];
    int n = 0;
    memset(data, '\0', sizeof(data));
    while (n < sizeof(data)) {
      int nn = read(0, data + n, sizeof(data) - n);
      if (nn <= 0)
        break;
      n += nn;
    }
    memdump(argv[1], data, n);
  } else {
    printf("Usage: memdump [format]\n");
    exit(1);
  }
  exit(0);
}

int
width(char fmt)
{
  switch (fmt) {
  case 'i':
    return 4;
  case 'p':
    return 8;
  case 'h':
    return 2;
  case 'c':
    return 1;
  case 's':
    return 8;
  case 'S':
    return 0;
  default:
    return 0;
  }
}

void
memdump(char *fmt, char *data, int len)
{
  int p = 0;
  for (; *fmt; fmt++) {
    int w = width(*fmt);
    if (p + w > len) {
      printf("memdump: not enough data\n");
      return;
    }
    switch (*fmt) {
    case 'i':
      printf("%d\n", *(int *)(data + p));
      break;
    case 'p':
      printf("%lx\n", *(uint64 *)(data + p));
      break;
    case 'h':
      printf("%d\n", *(short *)(data + p));
      break;
    case 'c':
      printf("%c\n", data[p]);
      break;
    case 's':
      printf("%s\n", *(char **)(data + p));
      break;
    case 'S':
      printf("%s\n", data + p);
      break;
    default:
      break;
    }
    p += w;
  }
}
