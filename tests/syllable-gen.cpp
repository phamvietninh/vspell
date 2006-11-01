#include <stdio.h>
char *vowels[] = {
	"o","u","i","y",
	NULL
};

char *last_consonants[] = {	// longest first
  "i","y","o","u","",
  NULL
};

char *padding_vowels[] = {
  "o","u","",
  NULL
};

int main()
{
  for (int i = 0;padding_vowels[i];i ++)
    for (int j = 0;vowels[j];j ++)
      for (int k = 0;last_consonants[k];k ++)
	printf("%s%s%s\n",
	       padding_vowels[i],
	       vowels[j],
	       last_consonants[k]);
  return 0;
}
