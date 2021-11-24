#include <sys/time.h>
#include <stdio.h>
 
int main() {
  int i = 0;
  struct timeval start, end;
  gettimeofday(&start, NULL);
 
  gettimeofday(&end, NULL);
  while(1){
    gettimeofday(&end, NULL);
    if (((end.tv_sec * 1000000 + end.tv_usec) 
      - (start.tv_sec * 1000000 + start.tv_usec)) >= 500000){
        printf("tv_sec = %lld\n", end.tv_sec);
        printf("tv_usec = %lld\n", end.tv_usec);
        printf("%d * 0.5 seconds passed\n", ++i);
        gettimeofday(&start, NULL);
    }
  }
  return 0;
}

