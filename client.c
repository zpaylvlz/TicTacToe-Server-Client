#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <string.h>

typedef struct sockaddr *sockaddrp;
int sockfd;


void *recv_other(void *arg){
	char buf[255]= {};
	
	while(1){
	    int ret = recv(sockfd,buf,sizeof(buf),0);
		if(0 > ret){
			perror("recv");
			pthread_exit(0);
		}
		printf("%s\n",buf);
		memset(buf, 0, sizeof(buf));
	}
}


 
int main()
{


	sockfd = socket(AF_INET,SOCK_STREAM,0);
	if(0 > sockfd)
	{
	    perror("socket");
	    return -1;
	}

	 
	struct sockaddr_in addr = {AF_INET};
	addr.sin_port = htons(5566);
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	socklen_t addr_len = sizeof(addr);



	int ret = connect(sockfd,(sockaddrp)&addr,addr_len);
	if(0 > ret){
		perror("connect");
		return -1;
	}


	char buf[255] = {};
	char name[255] = {};
	printf("enter your User name:：");
	scanf("%s",name);
	ret = send(sockfd,name,strlen(name),0);
	if(0 > ret){
	    perror("connect");
	    return -1;
	}
	 

	pthread_t tid;
	ret = pthread_create(&tid,NULL,recv_other,NULL);

	if(0 > ret){
		perror("pthread_create");
		return -1;
	}

	while(1){
		scanf("%s",buf);
		int ret = send(sockfd,buf,strlen(buf),0);
		if(0 > ret){
			perror("send");
			return -1;
		}

		if(0 == strcmp("quit",buf)){
			printf("%s,You've exit\n",name);
			return 0;
		}
		memset(buf, 0, sizeof(buf));
	}

}
