#include <stdio.h>

void main()
{
  int n,i,j,k;
  int *iterator;
  int *limit;

  n = 5;
  iterator = new int[n];
  limit = new int[n];
  for (i = 0;i < n;i ++) {
    iterator[i] = 0;
  }

  //#define MAX 2
#define MAX(i) limit[i]

  limit[0] = 2;
  limit[1] = 1;
  limit[2] = 3;
  limit[3] = 2;
  limit[4] = 2;

  bool run = true;
  i = 0;
  while (run) {
    if (i < n-1 && iterator[i] < MAX(i)) {
      i ++;
    } else {
      if (i == n-1 && iterator[i] < MAX(i)) {
	for (k = 0;k < n;k ++)
	  printf("%d ",iterator[k]);
	printf("\n");
	iterator[i] ++;
      } else {
	k = i;
	while (i >= 0 && iterator[i] == MAX(i))
	  i --;
	if (i < 0)
	  run = false;
	else {
	  iterator[i] ++;
	  iterator[k] = 0;
	}
      }
    }
  }

}
