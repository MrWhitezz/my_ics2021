#include <unistd.h>
#include <stdio.h>

int main() {
  write(1, "NHello World!\n", 14);
  int i = 2;
  volatile int j = 0;
  while (1) {
    j ++;
    if (j == 10000) {
      printf("%dth time! Hello World from Navy-apps for the \n", i ++);
      j = 0;
    }
  }
  return 0;
}
