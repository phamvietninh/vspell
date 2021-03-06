#include <stdio.h>
#include <string.h>

int main(int argc,char **argv)
{
	char w1[750],w2[750],w3[750];
	char ow1[750],ow2[750],ow3[750];
	double count,total_count;
	total_count = 0;
	int n_digit = 2;
	long long int base;

	if (argc == 2)
		sscanf(argv[1],"%d",&n_digit);
	for (base = 1;n_digit > 0; n_digit --) base *= 10;

	while (scanf("%s %s %lf",w1,w2,&count) == 3) {
		if (!strcmp(w1,ow1) && !strcmp(w2,ow2))
			total_count += count;
		else {
			if (total_count > 0)
				printf("%s %s %Ld\n",ow1, ow2, (long long int)(total_count*base) > 0 ? (long long int)(total_count*base) : (long long int)1);
			strcpy(ow1,w1);
			strcpy(ow2,w2);
			total_count = count;
		}
	}

	/*
	  Force to long long int because <s> <digit>/<opaque>/<punct> 
	  can easily run over int limit
	*/
	if ((long long int)(total_count*base) > 0)
		printf("%s %s %Ld\n",ow1, ow2, (long long int)(total_count*base) > 0 ? (long long int)(total_count*base) : (long long int)1);
	return 0;
}
