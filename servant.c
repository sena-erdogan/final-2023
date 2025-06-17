#include "helper.h"

static int handled = 0;

struct Node *head = NULL;
struct Node *current = NULL;

static int req=0;

static void handler(int sig){
	handled++;
	perror("SIGINT received, server ending");
}

int main(int argc, char *argv[]){

	clock_t t;
	t = clock();
	
	time_t ct = time(NULL);
	struct tm *tm;
	
	int i, j, from, to, port;
	char* ip;
	char* temp;
	char* temp_from;
	char* temp_to;
	char *dp;
	
	ip = (char*)malloc(15);
	temp_to = (char*)malloc(5);
	temp_from = (char*)malloc(5);
	temp = (char*)malloc(10);
	dp = (char*)malloc(20);
	
	struct sigaction sa;
	
	sigemptyset(&sa.sa_mask);
	
	sa.sa_flags = 0;
	sa.sa_handler = handler;
	
	if(sigaction(SIGINT, &sa, NULL) == -1)	exit(0);
	
	if(argc != 9){
		perror("\nUsage: ./servant -d directoryPath -c 10-19 -r IP -p PORT\n");
		return 1;
	}
	
	for(i=1; i<=7; i+=2){
		if(strcmp(argv[i], "-d") == 0){
			strcpy(dp, argv[i+1]);
			/*if((dp = open(argv[i+1], READ_FLAGS)) == -1){
				perror("Failed to open input file\n");
				return 1;
			}*/
		}else if(strcmp(argv[i], "-c") == 0){
			j=0;
			
			strcpy(temp, argv[i+1]);
			
			while(temp[j] != '-'){
				strncat(temp_from, &temp[j], 1);
				j++;
			}
			
			from = atoi(temp_from);
			
			j++;
			while(temp[j] != '\0'){
				strncat(temp_to, &temp[j], 1);
				j++;
			}
			
			to = atoi(temp_to);
			
			/*if(C<=4){
				perror("C should be larger than 4");
				exit(1);
			}*/
		}else if(strcmp(argv[i], "-r") == 0){
			strcpy(ip, argv[i+1]);
		}else if(strcmp(argv[i], "-p") == 0){
			port = atoi(argv[i+1]);
			/*if(C<=4){
				perror("C should be larger than 4");
				exit(1);
			}*/
		}else{
			perror("\nInvalid command\nformat: ./servant -d directoryPath -c 10-19 -r IP -p PORT\n");
			return 1;
		}
	}
	
	struct dirent **list;
	int num;
	
	num = scandir(dp, &list, NULL, alphasort);
	
	for(int i=2; i<to+2; i++){
		list[i-2] = list[from-1+i];
	}
	
	num = to-from+1;
	
	tm = localtime(&ct);
	printf("%s", asctime(tm));
	printf("Servant %d: loaded dataset, cities %s-%s\n", 0, list[0]->d_name, list[num-1]->d_name);
	
	DIR *d;
	
	struct dirent *dir;
	
	int fp;
	
	free(temp);
	
	char *bp;
	char *buf;
	
	buf=(char*)malloc(1);
	
	int fnum=0; // file number
	
	head = (struct Node*)malloc(sizeof(struct Node));
	current = (struct Node*)malloc(sizeof(struct Node));
	current = head;
	
	for(int i=0; i<num; i++){
		
		d = opendir(list[i]->d_name);
		
		int bytesread=0;
		
		if(d){
			while((dir = readdir(d)) != NULL){
			
				temp = (char*)malloc(20);
			
				fp = open(dir->d_name, READ_FLAGS, 0);
				
				current->date = dir->d_name;
				
				while(((bytesread = read(fp, buf, 1)) == -1) && (errno == EINTR)) ;
				if(bytesread <= 0)
					break;
				
				bp = buf;
				
				while(bytesread > 0){
					
					for(;;){
						if (bytesread <= 0)	break;
					
						if(buf[0] != ' ')	strncat(temp, buf, 1);
						else	break;
						
						while(((bytesread = read(fp, buf, 1)) == -1) && (errno == EINTR)) ;
					}
					
					current->id = temp;
					
					printf("%s %s\n", temp, current->id);
					
					while(((bytesread = read(fp, buf, 1)) == -1) && (errno == EINTR)) ;
					
					bzero(temp, 20);
					
					for(;;){
						if (bytesread <= 0)	break;
					
						if(buf[0] != ' ')	strncat(temp, buf, 1);
						else	break;
						
						while(((bytesread = read(fp, buf, 1)) == -1) && (errno == EINTR)) ;
					}
					
					current->type = temp;
					
					while(((bytesread = read(fp, buf, 1)) == -1) && (errno == EINTR)) ;
					
					bzero(temp, 20);
					
					for(;;){
						if (bytesread <= 0)	break;
					
						if(buf[0] != ' ')	strncat(temp, buf, 1);
						else	break;
						
						while(((bytesread = read(fp, buf, 1)) == -1) && (errno == EINTR)) ;
					}
					
					current->street = temp;
					
					while(((bytesread = read(fp, buf, 1)) == -1) && (errno == EINTR)) ;
					
					bzero(temp, 20);
					
					for(;;){
						if (bytesread <= 0)	break;
					
						if(buf[0] != ' ')	strncat(temp, buf, 1);
						else	break;
						
						while(((bytesread = read(fp, buf, 1)) == -1) && (errno == EINTR)) ;
					}
					
					current->surface = temp;
					
					while(((bytesread = read(fp, buf, 1)) == -1) && (errno == EINTR)) ;
					
					bzero(temp, 20);
					
					for(;;){
						if (bytesread <= 0)	break;
					
						if(buf[0] != '\n')	strncat(temp, buf, 1);
						else	break;
						
						while(((bytesread = read(fp, buf, 1)) == -1) && (errno == EINTR)) ;
					}
					
					current->price = temp;
					
					while(((bytesread = read(fp, buf, 1)) == -1) && (errno == EINTR)) ;
					
					bzero(temp, 20);
					
				}
				
				free(temp);
				
				if(close(fp) == -1){
					perror("Failed to close input file\n");
					return 1;
				}else	printf("input file closed\n");
				
				current = current->next;
				current = (struct Node*)malloc(sizeof(struct Node));
			}
		}
	
	}
	
	current = NULL;
	
	free(buf);
	free(bp);
	
	int sockfd;
	int connfd;
	struct sockaddr_in servaddr;
	
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
	
	buf = malloc(200);
	
	current = head;
	
	while(current != NULL){
	
		for(i=0; i<200; i++)	buf[i] = '\0';
		
		strcat(buf, current->date);
		strcat(buf, " ");
		strcat(buf, current->id);
		strcat(buf, " ");
		strcat(buf, current->type);
		strcat(buf, " ");
		strcat(buf, current->street);
		strcat(buf, " ");
		strcat(buf, current->surface);
		strcat(buf, " ");
		strcat(buf, current->price);
		strcat(buf, "\0");
	
		write(sockfd, buf, strlen(buf));
	
	}
	
	free(buf);
	
	//free(r);
	
	close(sockfd);
	
	for(int i=0; i<num; i++)	free(list[i]);
	
	free(list);
	
	free(dp);
	free(ip);
	free(temp_from);
	free(temp);
	free(temp_to);
	
	/*if(close(dp) == -1){
		perror("Failed to close input file\n");
		return 1;
	}*/
	
}
