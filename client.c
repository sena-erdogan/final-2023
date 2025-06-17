#include "helper.h"

pthread_mutex_t lock;

int port;
char* ip;

int sockfd;
int connfd;
struct sockaddr_in servaddr;

int count = -1;

char *type;
char *d1;
char *d2;
char *city;

void* thread(){

	clock_t t;
	t = clock();
	
	pthread_mutex_lock(&lock);
	
	time_t ct = time(NULL);
	struct tm *tm;
	
	count++;
	
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		printf("socket creation failed...\n");
		exit(0);
	}
	
	bzero(&servaddr, sizeof(servaddr));
   
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr(ip);
	servaddr.sin_port = htons(port);
   
	if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
		printf("connection with the server failed...\n");
		exit(0);
	}
	
	/*struct request *r = malloc(sizeof(struct request));
	
	r->type = type;
	r->d1 = d1;
	r->d2 = d2;
	if(strlen(city) > 0)	r->city = city;*/
	
	tm = localtime(&ct);
	
	printf("%s", asctime(tm));
	printf("Client-Thread-%d: I am requesting \"/transactionCount %s %s %s", count, type, d1, d2);
	if(strlen(city) > 0)	 printf(" %s\"\n", city);
	else	printf("\"\n");
	
	int i;
	
	char *buf = malloc(200);
	
	for(i=0; i<200; i++)	buf[i] = '\0';
	
	strcat(buf, type);
	strcat(buf, " ");
	strcat(buf, d1);
	strcat(buf, " ");
	strcat(buf, d2);
	if(strlen(city) > 0){
		strcat(buf, " ");
		strcat(buf, city);
		strcat(buf, "\0");
	}else	strcat(buf, "\0");
	
	write(sockfd, buf, strlen(buf));
	
	//free(r);
	
	//read from socket
	
	tm = localtime(&ct);
	
	printf("%s", asctime(tm));
	printf("Client-Thread-%d: The server's response to  \"/transactionCount %s %s %s", count, type, d1, d2);
	if(strlen(city) > 0)	 printf(" %s\" is %d\n", city, 0);
	else	printf("\" is %d\n", 0);
	
	free(buf);
	
	//printf("- Thread %d has reached the rendezvous point in %lf seconds\n", count, ((double)t)/CLOCKS_PER_SEC);
	
	pthread_mutex_unlock(&lock);
	
	return 0;
}

int main(int argc, char *argv[]){
	
	int i, j, fp, from, to;
	
	clock_t t;
	t = clock();
	
	time_t ct = time(NULL);
	struct tm *tm;
	
	ip = (char*)malloc(15);
	
	if(argc != 7){
		perror("\nUsage: ./client -r requestFile -q PORT -s IP\n");
		return 1;
	}
	
	for(i=1; i<=5; i+=2){
		if(strcmp(argv[i], "-r") == 0){
			if((fp = open(argv[i+1], READ_FLAGS, 0)) == -1){
				perror("Failed to open request file\n");
				return 1;
			}
		}else if(strcmp(argv[i], "-q") == 0){
			port = atoi(argv[i+1]);
			/*if(C<=4){
				perror("C should be larger than 4");
				exit(1);
			}*/
		}else if(strcmp(argv[i], "-s") == 0){
			strcpy(ip, argv[i+1]);
		}else{
			perror("\nInvalid command\nformat: ./client -r requestFile -q PORT -s IP\n");
			return 1;
		}
	}
	
	pthread_t* th;
	
	int t_num = 0;
	
	th = (pthread_t*)malloc(sizeof(pthread_t)*t_num);
	
	char *bp;
	char *buf;
	char *temp;
	int bytesread = 0;
	
	char check[18] = "transactionCount";
	
	int cityflag; /* 1 if there is a city at the end of the request */
	
	buf=(char*)malloc(1);
	
	while(((bytesread = read(fp, buf, 1)) == -1) && (errno == EINTR)) ;
	
	for( ; ; ){
		
		temp = (char*)malloc(50);
		for(i=0; i<50; i++)	temp[i] = '\0';
		
		bytesread=1;
	
		while(buf[0] == ' ' || buf[0] == '\n'){
			while(((bytesread = read(fp, buf, 1)) == -1) && (errno == EINTR)) ;
			if(bytesread <= 0)	break;
		}
		
		if(bytesread <= 0)	break;
		
		bp = buf;
		temp[0] = buf[0];
		
		while(bytesread > 0){

			if(temp[0] == check[0]){
				for(j=1; j<16; j++){
				
					while(((bytesread = read(fp, buf, 1)) == -1) && (errno == EINTR)) ;
					strncat(temp, buf,1);
					
					if (bytesread <= 0 || buf[0] != check[j])	perror("Invalid request\n");
				}
				
				while(((bytesread = read(fp, buf, 1)) == -1) && (errno == EINTR)) ;
				
				if(strcmp(temp, check) == 0){
					type = (char*)malloc(50);
					for(i=0; i<50; i++)	type[i] = '\0';
					while(bytesread > 0){
						while(((bytesread = read(fp, buf, 1)) == -1) && (errno == EINTR)) ;
						
						strncat(type, buf,1);
						
						if (bytesread <= 0 || buf[0] == ' ')	break;
					}
					
					d1 = (char*)malloc(15);
					for(i=0; i<15; i++)	d1[i] = '\0';
					while(bytesread > 0){
						while(((bytesread = read(fp, buf, 1)) == -1) && (errno == EINTR)) ;
						
						strncat(d1, buf,1);
						
						if (bytesread <= 0 || buf[0] == ' ')	break;
					}
					
					d2 = (char*)malloc(15);
					for(i=0; i<15; i++)	d2[i] = '\0';
					while(bytesread > 0){
						while(((bytesread = read(fp, buf, 1)) == -1) && (errno == EINTR)) ;
						
						strncat(d2, buf,1);
						
						if (bytesread <= 0 || buf[0] == ' ' || buf[0] == '\n')	break;
					}
					
					cityflag=1;
					
					while(buf[0] == ' ' || buf[0] == '\n'){
						if(buf[0] == '\n')	cityflag = 0;
						while(((bytesread = read(fp, buf, 1)) == -1) && (errno == EINTR)) ;
					}
					
					city = (char*)malloc(50);
					for(i=0; i<50; i++)	city[i] = '\0';
					
					if(cityflag == 1){
						
						strncat(city, buf,1);
						
						while(bytesread > 0){
							while(((bytesread = read(fp, buf, 1)) == -1) && (errno == EINTR)) ;
							
							if (bytesread <= 0 || buf[0] == ' ' || buf[0] == '\n')	break;
							else	strncat(city, buf,1);
							
						}
					}
					
					t_num++;
					
					printf("%s ",type);
					printf("%s ",d1);
					printf("%s ",d2);
					printf("%s\n",city);
					
					th = (pthread_t*)realloc(th, sizeof(pthread_t)*t_num);
					
					if (pthread_create(&th[t_num-1], NULL, &thread, NULL) != 0) {
						perror("Failed to create thread");
					}
					
					tm = localtime(&ct);
	
					printf("%s", asctime(tm));
					printf("Client-Thread-%d: Thread-%d has been created\n", t_num-1, t_num-1);
					
					free(type);
					free(d1);
					free(d2);
					free(city);
				}
			}
			bytesread = 0;
		}
		free(temp);
	}
	
	tm = localtime(&ct);
	
	printf("%s", asctime(tm));
	printf("Client: I have loaded %d requests and I'm creating %d threads\n", t_num, t_num);
	
	for(i = 0; i < t_num; i++){
		if(pthread_join(th[i], NULL) != 0)	perror("Failed to join thread");
		else{
			tm = localtime(&ct);
			printf("%s", asctime(tm));
			printf("Client-Thread-%d: Terminating\n", i);
		}
	}
	
	tm = localtime(&ct);
	printf("%s", asctime(tm));
	printf("All threads have terminated, goodbye\n");
	
	free(buf);	
	
	close(sockfd);
	
	
	free(ip);
	
	free(th);
	
	if(close(fp) == -1){
		perror("Failed to close request file\n");
		return 1;
	}else	printf("request file closed\n");
	
	return 0;
}
