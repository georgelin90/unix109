#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <stdlib.h>
int main(int argc, char *argv[]) {
/*	int i, ch;

	while((ch = getopt(argc, argv, "c:t:f:")) != -1) {
		switch(ch) {
		case 'c':
			printf("option c: %s\n", optarg);
			break;
		case 't':
			printf("option i: %s\n", optarg);
			break;
		case 'f':
			printf("option f: %s\n", optarg);
			break;
		case '?':
		case ':':
		default:
			printf("bad option: %c\n", ch);
			break;
		}
	}*/
	char *dirp = "/proc";
	struct dirent **restrict namelist;
	int n;
	n = scandir( dirp, &namelist, NULL, alphasort);	
	printf("%d\n", n);
	if( n == -1) 
	{
		perror("scandir");
		exit( EXIT_FAILURE);
	}
	int i = 0;
	while(namelist[i]->d_name[0]-'0'<10)
	{
		printf("%s\n", namelist[i]->d_name);
		i++;
	}

	for(int i = 0; i < n; i++)
	{
		free(namelist[n]);
	}
	exit(EXIT_SUCCESS);
/*
	argc -= optind;
	argv += optind;

	for(i = 0; i < argc; i++) {
		printf("argv[%d] = %s\n", optind + i, argv[i]);
	}
*/
	return 0;
}

