#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 30
void error_handling(char *message);

int main(int argc, char *argv[])
{	
	if(argc!=3){
		printf("Usage : %s <IP> <port>\n", argv[0]);
		exit(1);
	}
	
	int sock;
	sock=socket(PF_INET, SOCK_DGRAM, 0);   
	if(sock==-1)
		error_handling("socket() error");
	
	struct sockaddr_in serv_adr;
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family=AF_INET;
	serv_adr.sin_addr.s_addr=inet_addr(argv[1]);
	serv_adr.sin_port=htons(atoi(argv[2]));
	
	char message[BUF_SIZE];
	struct sockaddr_in from_adr;
	socklen_t adr_sz;
	int str_len;
	while(1)
	{
		fputs("Insert message(q to quit): ", stdout);
		fgets(message, sizeof(message), stdin);     
		if(!strcmp(message,"q\n") || !strcmp(message,"Q\n"))	
			break;
		
		sendto(sock, message, strlen(message), 0, 
					(struct sockaddr*)&serv_adr, sizeof(serv_adr));

		adr_sz=sizeof(from_adr);
		str_len=recvfrom(sock, message, BUF_SIZE, 0, 
					(struct sockaddr*)&from_adr, &adr_sz);

		message[str_len]='\0';//相当于在换行符后面追加一个空字符
		printf("Message from server: %s", message);
	}	
	close(sock);
	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}