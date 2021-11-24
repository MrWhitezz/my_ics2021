#include <stdint.h>
#include <sys/time.h>
#include <stdio.h>
uint32_t NDL_GetTicks();
int main() {
  int i = 0;
  uint32_t beg, end;
  beg = NDL_GetTicks();
  while(1){
    end = NDL_GetTicks();
    if (end - beg >= 500) {
        printf("%d * 0.5 seconds passed\n", ++i);
        beg = end;
    }
  }
  return 0;
}

