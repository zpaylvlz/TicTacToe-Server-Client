#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

typedef struct sockaddr *sockaddrp;

struct sockaddr_in src_addr[10];
socklen_t src_len = sizeof(src_addr[0]);


int confd[10] = {};


int count = 0;
char users[10][20];

void *broadcast(void *indexp){
	int index = *(int *)indexp; 
	char rcv_m[255] = {};
	char snd_m[255] = {};

	char uname[20] = {};

	int ret = recv(confd[index],uname,sizeof(uname),0);
	if (0 > ret){
		perror("recv");
		close(confd[index]);
		pthread_exit(0);
	}
	strcpy(users[index], uname);


	while(1){
		memset(rcv_m, 0, sizeof(rcv_m));
		memset(snd_m, 0, sizeof(snd_m));
		recv(confd[index],rcv_m,sizeof(rcv_m),0);

		if (0 == strcmp("quit",rcv_m)){
			printf("Trigger exit\n");
			sprintf(snd_m,"User %s has left\0",uname);
			
			for (int i = 0; i <= count; i++){
				if(i == index || 0 == confd[i])
					continue;
				send(confd[i],snd_m,strlen(snd_m),0);
			 memset(snd_m, 0, sizeof(snd_m));
			}
			
			confd[index] = 0;
			pthread_exit(0);
		}
		
		else if (0 == strcmp("su",rcv_m)){
			char getu[255] = {};
			sprintf(getu, "Online Users:\n");
			for (int i = 0; i <= count; i++){
				if(i == index || 0 == confd[i])
					continue;
				sprintf(getu, "%s%d:%s\n",getu, i,users[i]);
			}
			printf("to %d send %s\n",index,getu);
			sprintf(getu,"%s\0",getu);
			send(confd[index],getu,strlen(getu),0);
			 memset(getu, 0, sizeof(getu));
			
		}
		
		if (0 == strcmp("match",rcv_m)){
			char sel[255] = {};
			strcat(snd_m, "Which user you wanna play with:\n");
			send(confd[index],snd_m,strlen(snd_m),0);
			recv(confd[index],rcv_m,sizeof(rcv_m),0);

			bzero(snd_m,sizeof(snd_m));
			int m_en;
			sscanf(rcv_m, "%d", &m_en);

			sprintf(snd_m,"Accept challenge from %s?(yes/no)",users[index]);
			send(confd[m_en],snd_m,strlen(snd_m),0);
			bzero(snd_m,sizeof(snd_m));

			recv(confd[m_en],sel,sizeof(sel),0);


			printf("RCVM %s\n",sel);

			if (0 == strcmp("yes",sel)){

				sprintf(snd_m,"Match Start!\n");
				send(confd[index],snd_m,strlen(snd_m),0);
				send(confd[m_en],snd_m,strlen(snd_m),0);
				bzero(snd_m,sizeof(snd_m));

				char chess[255] = {};
				int att = index;
				int dfr = m_en;
				int isCtn = 1;
				int gcnt = 0;
				int gmap[10] = {-100,-100,-100,-100,-100,-100,-100,-100,-100,-100};
				while (isCtn){
					sprintf(snd_m,"Your Turn!\n");
					send(confd[att],snd_m,strlen(snd_m),0);
					bzero(snd_m,sizeof(snd_m));

					sprintf(snd_m,"Waiting for enemy...\n");
					send(confd[dfr],snd_m,strlen(snd_m),0);
					bzero(snd_m,sizeof(snd_m));


	
					recv(confd[att],chess,sizeof(chess),0);
					int cpl;
					sscanf(chess,"%d",&cpl);
					gmap[cpl] = att;
					gcnt++;
					if (gmap[1] + gmap[2] + gmap[3] == att * 3)
						isCtn= 0;
					if (gmap[4] + gmap[5] + gmap[6] == att * 3)
						isCtn = 0;
					if (gmap[7] + gmap[8] + gmap[9] == att * 3)
						isCtn = 0;
					if (gmap[1] + gmap[4] + gmap[7] == att * 3)
						isCtn = 0;
					if (gmap[2] + gmap[5] + gmap[8] == att * 3)
						isCtn = 0;
					if (gmap[3] + gmap[6] + gmap[9] == att * 3)
						isCtn = 0;
					if (gmap[1] + gmap[5] + gmap[9] == att * 3)
						isCtn = 0;
					if (gmap[3] + gmap[5] + gmap[7] == att * 3)
						isCtn = 0;

					sprintf(snd_m,"%d %d %d\n%d %d %d\n%d %d %d\n",gmap[1],gmap[2],gmap[3],gmap[4],gmap[5],gmap[6],gmap[7],gmap[8],gmap[9]);
						send(confd[index],snd_m,strlen(snd_m),0);
						send(confd[m_en],snd_m,strlen(snd_m),0);
						bzero(snd_m,sizeof(snd_m));

					if (gcnt != 9 && isCtn){
						int t = att;
						att = dfr;
						dfr = t;
					}
					else if (gcnt == 9 && isCtn){
						for (int i = 1; i <=9; i++ ) gmap[i] = -100;
						sprintf(snd_m,"Tie! Next Round is coming\n");
						send(confd[index],snd_m,strlen(snd_m),0);
						send(confd[m_en],snd_m,strlen(snd_m),0);
						bzero(snd_m,sizeof(snd_m));
						gcnt = 0;
					}
					
				}
				sprintf(snd_m,"%s won the game!\n", users[att]);
				send(confd[index],snd_m,strlen(snd_m),0);
				send(confd[m_en],snd_m,strlen(snd_m),0);
				bzero(snd_m,sizeof(snd_m));

				
				
				
			}
			else{
				
			}
			
		}

		for(int i = 0; i <= count; i++){
			if (i == index || 0 == confd[i])
				continue;
			
			printf("%d is online\n",i);
		}
	}
}



int main(){
	printf("Server Startup!\n");
	
	int sockfd = socket(AF_INET,SOCK_STREAM,0);

	if (0 > sockfd){
		perror("socket");
		return -1;
	}

	struct sockaddr_in addr = {AF_INET};
	addr.sin_port = htons(5566);
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	socklen_t addr_len = sizeof(addr);



	int ret = bind(sockfd,(sockaddrp)&addr,addr_len);
	if(0 > ret)
	{
	    perror("bind");
	    return -1;
	}

	listen(sockfd,10);
	int index = 0;

	while (count <= 10){
		confd[count] = accept(sockfd,(sockaddrp)&src_addr[count],&src_len);
		++count;
		index = count - 1;
		pthread_t tid;
		int ret = pthread_create(&tid,NULL,broadcast,&index);
		if (0>ret){
			perror("pthread_create");
			return -1;
		}
	}


}

