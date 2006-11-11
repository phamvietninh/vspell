#include <stdio.h>
#include <stdlib.h>

/*
 * This program take a text separated by new line
 * and extract specific paragraphs from that
 */
int main(int argc,char **argv)
{
	int from, to, i;
	char buf[1025];

	if (argc >= 2) {
		if (sscanf(argv[1],"%d",&from) != 1) {
			fprintf(stderr,"Invalid parameter (from): %s\n",argv[1]);
			exit(1);
		}
		if (argc == 3) {
			if (sscanf(argv[2],"%d",&to) != 1) {
				fprintf(stderr,"Invalid parameter (to): %s\n",argv[2]);
				exit(1);
			}
		}
		else
			to = from;
	}
	else {
		fprintf(stderr,"Syntax: pf <from> [<to>]\n");
		exit(1);
	}

	i = 0;
	while (fgets(buf,1025, stdin) && i <= to) {
		if (i >= from)
			fputs(buf, stdout);
		if (buf[0] == '\n')
			i ++;
	}
	return 0;
}
