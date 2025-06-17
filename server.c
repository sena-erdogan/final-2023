#include "helper.h"

static int handled = 0;

int count = 0;
int port;

struct sockaddr_in servaddr, client;

int connfd;
int sockfd;
int len;

char *type;
char *d1;
char *d2;
char *city;

pthread_mutex_t lock;

static void handler(int sig){
	handled++;
	perror("SIGINT received, server ending");
}

void* thread(){

	clock_t t;
	t = clock();
	
	pthread_mutex_lock(&lock);
	
	time_t ct = time(NULL);
	struct tm *tm;
	
	//struct request *r = malloc(sizeof(struct request));
	
	
	
	/*type = (char*)malloc(50);
	d1 = (char*)malloc(50);
	d2 = (char*)malloc(50);
	city = (char*)malloc(50);*/
	
	int i;
	
	for(;;){
		char *buf = malloc(50);
	
		bzero(buf, 50);
		
		read(sockfd, buf, sizeof(buf));
		
		count++;
		
        	tm = localtime(&ct);
		printf("%s", asctime(tm));
		printf("Request arrived \"transactionCount %s\"\n", buf);
		
		tm = localtime(&ct);
		printf("%s", asctime(tm));
		printf("Contacting servant %d\n", 0);
		
		/*printf("From client: %s ", r->type);
		printf("%s ", r->d1);
		printf("%s ", r->d2);
		if(strlen(r->city) > 0)	printf("%s\n", r->city);
		else	printf("\n");*/
	   
		//write(connfd, buff, sizeof(buff));
	   
		/*if (strncmp("exit", buff, 4) == 0) {
		    printf("Server Exit...\n");
		    break;
		}*/
		
		tm = localtime(&ct);
		printf("%s", asctime(tm));
		printf("Response received: %d, forwarded to client\n", 0);
		
		free(buf);
		break;
	}
		
	/*free(type);
	free(d1);
	free(d2);
	free(city);*/
	//free(r);
	
	tm = localtime(&ct);
	t = clock()-t;
	
	//printf("- Thread %d has reached the rendezvous point in %lf seconds\n", count, ((double)t)/CLOCKS_PER_SEC);
	
	pthread_mutex_unlock(&lock);
	
	return 0;
}

int main(int argc, char *argv[]){

	clock_t t;
	t = clock();
	
	time_t ct = time(NULL);
	struct tm *tm;

	int i, t_num;
	
	struct sigaction sa;
	
	sigemptyset(&sa.sa_mask);
	
	sa.sa_flags = 0;
	sa.sa_handler = handler;
	
	if(sigaction(SIGINT, &sa, NULL) == -1)	exit(0);
	
	if(argc != 5){
		perror("\nUsage: ./server -p PORT -t numberOfThreads\n");
		return 1;
	}
	
	for(i=1; i<=3; i+=2){
		if(strcmp(argv[i], "-p") == 0){
			port = atoi(argv[i+1]);
			/*if(C<=4){
				perror("C should be larger than 4");
				exit(1);
			}*/
		}else if(strcmp(argv[i], "-t") == 0){
			t_num = atoi(argv[i+1]);
			if(t_num<=5){
				perror("Thread number should be at least 5");
				exit(1);
			}
		}else{
			perror("\nInvalid command\nformat: ./server -p PORT -t numberOfThreads\n");
			return 1;
		}
	}
   
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	
	if(sockfd == -1) {
	        printf("socket creation failed...\n");
	        exit(0);
	}
    
	bzero(&servaddr, sizeof(servaddr));
   
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(port);
   
	if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
	        printf("socket bind failed...\n");
	        exit(0);
	}
	
	int num = 1;
	
	//setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &num, sizeof(num));

	if((listen(sockfd, 5)) != 0){
	        printf("Listen failed...\n");
	        exit(0);
	}
 
	len = sizeof(client);
	
	connfd = accept(sockfd, (SA*)&client, &len);
	if(connfd < 0){
		printf("server accept failed...\n");
		exit(0);
	}
   
	pthread_t* th;
	
	th = (pthread_t*)malloc(sizeof(pthread_t)*t_num);
	
	for (i = 0; i < t_num; i++){
		if (pthread_create(&th[i], NULL, &thread, NULL) != 0) {
			perror("Failed to create thread");
		}
	}
	
	for (i = 0; i < t_num; i++){
		if (pthread_join(th[i], NULL) != 0)	perror("Failed to join thread");
	}

	free(th);
	
	close(sockfd);
	
	tm = localtime(&ct);
	printf("%s", asctime(tm));
	printf("SIGINT has been received. I handled a total of %d requests. Goodbye.\n", count);
	
}
