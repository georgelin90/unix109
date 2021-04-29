#include <dlfcn.h>
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>

static int (*old_open)(const char *pathname, int flags, mode_t mode) = NULL;
static int (*old_close)(int fd) = NULL;
static FILE* (*old_fopen)(const char *pathname, const char *mode);
static int (*old_fclose)(FILE *stream);
int open(const char *pathname, int flags, mode_t mode) {
	int result;
	char *resolved;
        if(old_open == NULL) {
                void *handle = dlopen("libc.so.6", RTLD_LAZY);
                if(handle != NULL) {
                        old_open = dlsym(handle, "open");
                }
        }
        if(old_open != NULL) {
		result = old_open(pathname, flags, mode);
        }
	resolved = realpath(pathname, NULL);
	if(resolved == NULL)
	        fprintf(stderr, "open(\"%s\", %o, %o) = %d \n", pathname, flags, mode, result);
	else
		fprintf(stderr, "open(\"%s\", %o, %o) = %d \n", resolved, flags, mode, result);
	free(resolved);
        return 0;
}
FILE* fopen(const char *pathname, const char *mode){
	FILE* result;
	char *resolved;
        if(old_fopen == NULL) {
                void *handle = dlopen("libc.so.6", RTLD_LAZY);
                if(handle != NULL) {
                        old_fopen = dlsym(handle, "fopen");
                }
        }
        if(old_fopen != NULL) {
		result = old_fopen(pathname, mode);
        }
	resolved = realpath(pathname, NULL);
	if(resolved == NULL)
	        fprintf(stderr, "fopen(\"%s\", \"%s\") = %p \n", pathname, mode, result);
	else
		fprintf(stderr, "fopen(\"%s\", \"%s\") = %p \n", resolved, mode, result);
	free(resolved);
        return 0;
}
int close(int fd){
	int result;
	char path[0xFFF], *buf;
	ssize_t v;
	pid_t pid = getpid();
	if(old_close == NULL){
                void *handle = dlopen("libc.so.6", RTLD_LAZY);
                if(handle != NULL) {
                        old_close = dlsym(handle, "close");
                }
        }	
	//get filename from fd
	sprintf(path, "/proc/%d/fd/%d", pid, fd);
	buf = malloc(PATH_MAX);
	if((v = readlink(path, buf, PATH_MAX)) != -1)
		buf[v] = '\0';
	else{
		perror("readlink");
		exit(1);
	}
	if(old_close != NULL){
		result = old_close(fd);
	}
	fprintf(stderr, "close(\"%s\") = %d \n", buf, result);
}
int fclose(FILE *stream){
	int result, fd = fileno(stream);
	pid_t pid = getpid();
	char path[0xFFF], *buf;
	ssize_t v;
	if(old_fclose == NULL){
                void *handle = dlopen("libc.so.6", RTLD_LAZY);
                if(handle != NULL) {
                        old_fclose = dlsym(handle, "fclose");
                }
        }	
	//get filename from fd
	sprintf(path, "/proc/%d/fd/%d", pid, fd);
	buf = malloc(PATH_MAX);
	if((v = readlink(path, buf, PATH_MAX)) != -1)
		buf[v] = '\0';
	else{
		perror("readlink");
		exit(1);
	}
	if(old_fclose != NULL){
		result = old_fclose(stream);
	}
	fprintf(stderr, "fclose(\"%s\") = %d \n", buf, result);
}
