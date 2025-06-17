#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <signal.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <dirent.h>

#define SA struct sockaddr

#define READ_FLAGS O_RDONLY

struct request{
	char *type;
	char *d1;
	char *d2;
	char *city;
};

struct Node{
	char *date;
	char *id;
	char *type;
	char *street;
	char *surface;
	char *price;
	struct Node* next;
};
