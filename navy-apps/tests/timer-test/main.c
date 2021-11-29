#include <stdint.h>
#include <NDL.h>
#include <sys/time.h>
#include <stdio.h>
int main() {
  NDL_Init(0);
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

