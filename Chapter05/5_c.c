#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#define BUFSIZE 1024
void error_handling(char *message);

int main(int argc, char* argv[])
{
	int sock;
	struct sockaddr_in serv_addr;
	
	if(argc!=3){
		printf("Usage : %s <IP> <port>\n", argv[0]);
		exit(1);
	}
	
	sock=socket(PF_INET, SOCK_STREAM, 0);
	if(sock == -1)
		error_handling("socket() error");
	
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_addr.s_addr=inet_addr(argv[1]);
	serv_addr.sin_port=htons(atoi(argv[2]));
		
	if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1) 
		error_handling("connect() error!");

    char message[BUFSIZE];
    char recv_mesg[BUFSIZE];
    char str_len[5];
    int recv_len,strlen,recv_cnt;

	while(1) 
	{
		fputs("Input message(Q to quit)(example:0006Hello?): ", stdout);
		fgets(message, BUFSIZE, stdin);
		
		if(!strcmp(message,"q\n") || !strcmp(message,"Q\n"))
			break;

        //传递字符串长度
		write(sock, message,4);

        //传递字符串数据
		write(sock, message+4, BUFSIZE-1);

        //存字符串长度
        strncpy(str_len,message,4);
        str_len[4]='\0';
        strlen=atoi(str_len);

        printf("strlen=%d \n",strlen);
        recv_len=0;

        //读取strlen长度的数据
		while(strlen>recv_len)
		{
        puts("运行到此1");
			recv_cnt=read(sock, &recv_mesg[recv_len], BUFSIZE-1);
			recv_len+=recv_cnt;
        puts("运行到此2");        
		}

		recv_mesg[strlen]='\0';

        //***注***
        //要加换行符\n，因为服务端传送过来的数据不含换行符。
		printf("Message from server: %s \n", recv_mesg);

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
