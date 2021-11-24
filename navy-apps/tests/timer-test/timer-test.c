#include <sys/time.h>
#include <stdio.h>
 
int main() {
  int i = 0;
  struct timeval start, end;
  gettimeofday(&start, NULL);
 
  gettimeofday(&end, NULL);
  while(1){
        printf("tv_sec = %d\n", end.tv_sec);
        printf("tv_usec = %d\n", end.tv_usec);
        //????????????????????????????
    gettimeofday(&end, NULL);
    if (((end.tv_sec * 1000000 + end.tv_usec) 
      - (start.tv_sec * 1000000 + start.tv_usec)) >= 500000){
        printf("%d * 0.5 seconds passed\n", ++i);
        gettimeofday(&start, NULL);
    }
  }
  return 0;
}

