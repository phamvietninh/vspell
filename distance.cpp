#include <iostream>
#include <string>
#include <values.h>
#include <stdio.h>

#define MAX_WIDTH  32
#define MAX_HEIGHT 32

int a[MAX_WIDTH+2][MAX_HEIGHT+2];
#define A(i,j) a[(i)+2][(j)+2]
#define min2(a,b) ((a) > (b) ? (b) : (a))
#define min3(a,b,c) ((a) >= (b) && (c) >= (b) ? (b) : ((a) >= (c) && (b) >= (c) ? (c) : (a)))

int ed(const char *s1,int n1,const char *s2,int n2)
{
  static int k,i,j;		// static for speed

  // the following formula is used:
  // a(0,0) = 0                x[0] = y[0]
  // a(0,0) = 1                x[0] <> y[0]
  // a(0,j) = j                1 <= j <= n
  // a(i,0) = i                1 <= i <= m
  // a(i,j) = a(i-1,j-1)       x[i] = y[j]
  // a(i,j) = 1 + min{a(i-2,j-2),a(i,j-1),a(i-1,j)} 
  //                           x[i] = y[j-1]
  //                           x[i-1] = y[j] 
  // a(i,j) = 1 + min{a(i-1,j-1),a(i,j-1),a(i-1,j)} otherwise

  // case 1,2
  A(0,0) = s1[0] == s2[0] ? 0 : 1;
  printf("(%d,%d) = %d\n",1,1,A(0,0));

  // case 3,4 are coded on the boundary of a[][].

  // k go from upper left to upper right then lower right
  for (k = 1;k < n1+n2-1;k ++)
    // (i,j) go from upper right to lower left.
    for (i = min2(n1-1,k),j = k-i;i >= 0 && j < n2;--i,++j) {
      // case 5
      if (s1[i] == s2[j]) {
	A(i,j) = A(i-1,j-1);
	printf("(%d,%d) = %d\n",i+1,j+1,A(i,j));
	continue;
      }

      // case 6
      if ((i > 0 && s1[i-1] == s2[j]) || 
	  (j > 0 && s1[i] == s2[j-1])) {
	A(i,j) = min3(A(i-2,j-2),A(i,j-1),A(i-1,j)) + 1;
	printf("(%d,%d) < %d %d %d -> %d\n",i+1,j+1,A(i-2,j-2),A(i,j-1),A(i-1,j),A(i,j));
	continue;
      }

      // case 7
      A(i,j) = min3(A(i-1,j-1),A(i,j-1),A(i-1,j)) + 1;
      printf("(%d,%d) > %d %d %d -> %d\n",i+1,j+1,A(i-1,j-1),A(i,j-1),A(i-1,j),A(i,j));
    }

  return A(n1-1,n2-1);
}

int main()
{
  string s1,s2;
  int i,j;

  for (i = 0;i < MAX_WIDTH+2;i ++)
    a[i][0] = MAXINT;
  for (i = 0;i < MAX_HEIGHT+2;i ++)
    a[0][i] = MAXINT;
  a[1][1] = MAXINT;
  for (i = 0;i < MAX_WIDTH;i ++)
    a[i+2][1] = i+1;
  for (i = 0;i < MAX_HEIGHT;i ++)
    a[1][i+2] = i+1;

  while (cin >> s1 >> s2) {
    int ret = ed(s1.data(),s1.size(),s2.data(),s2.size());
    for (i = 0;i < s1.size();i ++) {
      for (j = 0;j < s2.size();j ++)
	cout << a[i+2][j+2] << " ";
      cout << endl;
    }
    cout << ret << endl;
  }
}
