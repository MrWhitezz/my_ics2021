#include <stdint.h>
#include <sys/time.h>
#include <stdio.h>
uint32_t NDL_GetTicks();
int main() {
  int i = 0;
  // struct timeval start, end;
  // struct timezone tz;
  // gettimeofday(&start, NULL);
  // gettimeofday(&end, NULL);
  uint32_t beg, end;
  beg = NDL_GetTicks();
  while(1){
        // printf("tv_sec = %d\n", end.tv_sec);
        // printf("tv_usec = %d\n", end.tv_usec);
        //????????????????????????????
    // gettimeofday(&end, NULL);
    end = NDL_GetTicks();
    // if (((end.tv_sec * 1000000 + end.tv_usec) 
    //   - (start.tv_sec * 1000000 + start.tv_usec)) >= 500000){
    if (end - beg >= 500) {
        printf("%d * 0.5 seconds passed\n", ++i);
        // gettimeofday(&start, NULL);
        beg = end;
    }
  }
  return 0;
}

