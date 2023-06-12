#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#define BUFSIZE 1024
void error_handling(char *message);

int main(int argc, char *argv[])
{
	int serv_sock;
	int clnt_sock;

	struct sockaddr_in serv_addr;
	struct sockaddr_in clnt_addr;
	socklen_t clnt_addr_size;

	char message[]="Hello World!";
	
	if(argc!=2){
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}
	
	serv_sock=socket(PF_INET, SOCK_STREAM, 0);
	if(serv_sock == -1)
		error_handling("socket() error");
	
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
	serv_addr.sin_port=htons(atoi(argv[1]));
	
	if(bind(serv_sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr))==-1)
		error_handling("bind() error"); 
	
	if(listen(serv_sock, 5)==-1)
		error_handling("listen() error");
	
	clnt_addr_size=sizeof(clnt_addr);  

    //***注***
    //保证str_len[4]为空字符'\0'，以配合函数atoi()
    char str_len[5]={'\0'};
    int strlen,recv_len,recv_cnt;

    char data[BUFSIZE];

#if 0
	for(int i=0; i<5; i++)
	{
		clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);	
        //读取表示字符串长度的char数组内容
		read(clnt_sock, &str_len, 4);
		strlen=atoi(str_len);

        //测试strlen的值
        printf("%d \n",strlen);
        
		recv_len=0;
        
        //读取strlen长度的数据
		while(strlen>recv_len)
		{
			recv_cnt=read(clnt_sock, &data[recv_len], BUFSIZE-1);
			recv_len+=recv_cnt;
		}        
        puts("运行到此1");        
        write(clnt_sock,&data[0],strlen);
        puts("运行到此2");        

        //为什么要写在这边？
		// close(clnt_sock);
	}
#endif

    //***注***
    //accept()不要写在for循环里面
    //如果写在里面，会导致服务端运行完了一遍完整的for循环体内容后
    //一直卡在accept()这边
    //原因：如果等待队列为空，accpet()函数会阻塞
	clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);	
	for(int i=0; i<5; i++)
	{
        //读取表示字符串长度的char数组内容
		read(clnt_sock, &str_len, 4);
		strlen=atoi(str_len);

        //测试strlen的值
        printf("%d \n",strlen);
        
		recv_len=0;
        
        //读取strlen长度的数据
		while(strlen>recv_len)
		{
			recv_cnt=read(clnt_sock, &data[recv_len], BUFSIZE-1);
			recv_len+=recv_cnt;
		}        
        puts("运行到此1");        
        write(clnt_sock,&data[0],strlen);
        puts("运行到此2");        

        //***注***
        //不要写在此处
		// close(clnt_sock);
	}

    close(clnt_sock);

	close(serv_sock);
	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
