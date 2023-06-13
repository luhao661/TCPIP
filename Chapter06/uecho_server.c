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
	if(argc!=2){
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}
	
	//创建服务端套接字文件描述符
	int serv_sock;

	//创建套接字，用文件描述符serv_sock表示
	serv_sock=socket(PF_INET, SOCK_DGRAM, 0);
	if(serv_sock==-1)
		error_handling("UDP socket creation error");
	
    //创建服务器端的sockaddr_in结构布局的结构体变量，以便存储IP和端口，
    //最后再变为地址，用指向sockaddr结构体的指针读取
	struct sockaddr_in serv_adr;
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family=AF_INET;
	serv_adr.sin_addr.s_addr=htonl(INADDR_ANY);
	serv_adr.sin_port=htons(atoi(argv[1]));
	
    //给套接字分配IP地址和端口
	if(bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr))==-1)
		error_handling("bind() error");

    //创建客户端的sockaddr_in结构布局的结构体变量，
    //最后再变为地址，用指向sockaddr结构体的指针存地址，
    //以便写入客户端的IP和端口
	struct sockaddr_in clnt_adr;
	socklen_t clnt_adr_sz;
	//创建用于保存接收的数据的字符数组
	char message[BUF_SIZE];
	int str_len;
	while(1) 
	{
		clnt_adr_sz=sizeof(clnt_adr);
		str_len=recvfrom(serv_sock, message, BUF_SIZE, 0, 
			(struct sockaddr*)&clnt_adr, &clnt_adr_sz);
			//(struct sockaddr*)&clnt_adr是用于存储发送端的
			//地址信息
			//&clnt_adr_sz是含发送端地址信息的结构体变量的
			//长度
			//str_len为传输的字节数
		sendto(serv_sock, message, str_len, 0, 
			(struct sockaddr*)&clnt_adr, clnt_adr_sz);
	}	
	close(serv_sock);
	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
