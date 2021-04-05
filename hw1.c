#define _GNU_SOURCE
#include <inttypes.h>
#include <sys/types.h>
#include <pwd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
int inodenum[1000], lastpid = 0;

void readcontent(char *name)
{
	FILE* fp;
	char path[20] = "/proc/";
	strncat(path, name, 4);
	//read comm
	char commpath[20] = "\0";
	for(int i = 0; i < strlen(path); i++)
	{
		commpath[i] = path[i];
	}
	char temp[20] = "/comm";
	strncat(commpath, temp, 6);
	fp = fopen(commpath, "r");
	char comm[100];
	if(fp == NULL)
	{
		perror("fopen");
		exit(EXIT_FAILURE);
	}
	if(fp != NULL)
	{
		fgets(comm,100, fp);
		comm[strcspn(comm, "\n")] = 0;
	}
	fclose(fp);
	//read uid
	ssize_t bufsiz = PATH_MAX;
	char uidpath[20] = "\0", uidtemp[20] = "/loginuid", *uidbuf;
	uidbuf = malloc(bufsiz);
	for(int i = 0; i < strlen(path); i++)
	{
		uidpath[i] = path[i];
	}
	strncat(uidpath, uidtemp, 9);
	fp = fopen(uidpath, "r");
	char userid[100];
	if(fp != NULL)
	{
		fgets(userid, 100, fp);
	}
        fclose(fp);
	unsigned int nid = atoi(userid);
	if(nid == 4294967295)
		nid = 0;
	struct passwd *pws;
	pws = getpwuid(nid);
	//read cwd
	char cwdpath[20] = "\0", temp2[20] = "/cwd", *buf;
	buf = malloc(bufsiz);
	for(int i = 0; i < strlen(path); i++)
	{
		cwdpath[i] = path[i];
	}
	strncat(cwdpath, temp2, 4);
	ssize_t v; 
	if((v = readlink(cwdpath, buf, bufsiz)) != -1)
	{
		buf[v] = '\0';
	}
	else
	{
		printf("%s(Permission Denied)\n", cwdpath);
	//	perror("readlink");
	//	exit(EXIT_FAILURE);
	}
	//print cwd
	struct stat st;
	stat(cwdpath, &st);
	printf("%s\t%s\t%s\tcwd\tDIR\t%lu\t%s\n", comm, name, pws->pw_name, st.st_ino, buf);
	//printf("inode: %u\n", st.st_ino);
	//read root	
	char rootpath[20] = "\0", temp3[20] = "/root", *buf2;
	buf2 = malloc(bufsiz);
	for(int i = 0; i < strlen(path); i++)
	{
		rootpath[i] = path[i];
	}
	strncat(rootpath, temp3, 5);
	ssize_t v2; 
	if((v2 = readlink(rootpath, buf2, bufsiz)) != -1)
	{
		buf2[v2] = '\0';
	}
	else
	{
		printf("%s(Permission Denied)\n", rootpath);
	//	perror("readlink");
	//	exit(EXIT_FAILURE);
	}
	struct stat st2;
	stat(rootpath, &st2);
	//printf root
	printf("%s\t%s\t%s\troot\tDIR\t%lu\t%s\n", comm, name, pws->pw_name, st2.st_ino, buf2);
	//read exe	
	char exepath[20] = "\0", temp4[20] = "/exe", *buf3;
	buf3 = malloc(bufsiz);
	for(int i = 0; i < strlen(path); i++)
	{
		exepath[i] = path[i];
	}
	strncat(exepath, temp4, 4);
	ssize_t v3; 
	if((v3= readlink(exepath, buf3, bufsiz)) !=-1)
	{
		buf3[v3] = '\0';
	}
	else
	{
		printf("%s(Permission Denied)\n", exepath);
	//	perror("readlink");
	//	exit(EXIT_FAILURE);
	}
	struct stat st3;
	stat(exepath, &st3);
	//print exe
	printf("%s\t%s\t%s\texe\tREG\t%ju\t%s\n", comm, name, pws->pw_name, (uintmax_t)st3.st_ino, buf3);
	//read maps
	char mapspath[20] = "\0", temp5[20] = "/maps";	
	for(int i = 0; i < strlen(path); i++)
	{
		mapspath[i] = path[i];
	}
	char data[100];
	strncat(mapspath, temp5, 5);
	fp = fopen(mapspath, "r");
	fseek(fp, 0, SEEK_END);
	long fsize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	int index = 0;
	int in, flag = 1;
	char *token, *token2;
	const char s[2] = " ";
	for(int i =0; i < 1000; i++)
		inodenum[i] = 0;
	while(1){
	fseek(fp, 46, SEEK_CUR);
	fgets(data, 100, fp);
	token = strtok(data, s);
	token2 = strtok(NULL, s);
	//printf("%s\t%s", token, token2);
	in = atoi(token);
	flag = 1;
//	printf("inode: %d", in);
	for(int i = 0; i < 1000; i++)
	{
		if(in == inodenum[i])
		{
		//	printf("old\t%d\n", in);
			flag = 0;
			break;
		}
	}
	if(flag == 1)//print maps
	{
		inodenum[index] = in;
		index++;
		printf("%s\t%s\t%s\tmem\tREG\t%s\t%s",comm, name, pws->pw_name,  token, token2);
	}
	else
	{
		;
	}
	if(feof(fp))
		break;
	}
	fclose(fp);
	//read fd
	char fdpath[20] = "\0", temp6[20] = "/fd";
	for(int i = 0; i < strlen(path); i++)
	{
		fdpath[i] = path[i];
	}
	strncat(fdpath, temp6, 3);
	DIR *dir;
	struct dirent *dent;
	dir = opendir(fdpath);
	char *buf4,fdlink[20];
	int dfd;
	ssize_t v4;
	struct stat st4;
	if(dir != NULL)
	{
		int cnt =0;
		while((dent = readdir(dir)) !=NULL)
		{
			if(cnt < 2)
			{
				cnt++;
				continue;
			}
			printf("%s\n", dent->d_name);
			dfd = dirfd(dir);
			for(int i = 0; i < strlen(fdpath); i++)
				fdlink[i] = fdpath[i];
			fdlink[strlen(fdlink)] = '/';
			strcat(fdlink, dent->d_name);
			stat(fdlink, &st4);
//			printf("inode: %u\n", st4.st_ino);
/*			if((v4 = readlink(fdlink, buf4, bufsiz)) != -1)
				buf4[v4] = '\0';
			else
			{
				printf("????");
			//	perror("readlink");
			//	exit(EXIT_FAILURE);
			}*/
			//printf("%s\n", buf4);
			switch (st4.st_mode & S_IFMT) {
           case S_IFBLK:  printf("block device\n");            break;
           case S_IFCHR:  printf("character device\n");        break;
           case S_IFDIR:  printf("directory\n");               break;
           case S_IFIFO:  printf("FIFO/pipe\n");               break;
           case S_IFLNK:  printf("symlink\n");                 break;
           case S_IFREG:  printf("regular file\n");            break;
           case S_IFSOCK: printf("socket\n");                  break;
           default:       printf("unknown?\n");                break;
           }
		}
	}
}
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
	n = scandir( dirp, &namelist, NULL, versionsort);	
	if( n == -1) 
	{
		perror("scandir");
		exit( EXIT_FAILURE);
	}
	int i = 2;
	while(namelist[i]->d_name[0]-'0'<10)
	{
		readcontent(namelist[i]->d_name);
		i++;
	}
	lastpid = i;
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

