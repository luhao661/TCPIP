#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
void error_handling(char *message);

int main(int argc, char *argv[]) 
{
	int tcp_sock, udp_sock;
	
	//创建TCP套接字
	tcp_sock=socket(PF_INET, SOCK_STREAM, 0);
	//创建UDP套接字
	udp_sock=socket(PF_INET, SOCK_DGRAM, 0);	

	printf("SOCK_STREAM: %d \n", SOCK_STREAM);
	printf("SOCK_DGRAM: %d \n", SOCK_DGRAM);
	
	int sock_type;
	socklen_t optlen;
	optlen=sizeof(sock_type);
	int state;

	//套接字文件描述符    可选项所属协议层   要查看的可选项
	state=getsockopt(tcp_sock, SOL_SOCKET, SO_TYPE, 
	(void*)&sock_type, &optlen);
	//用于保存结果的变量的地址   变量的长度
	//成功时返回 0
	
	if(state)
		error_handling("getsockopt() error!");
	printf("Socket type one: %d \n", sock_type);
	
	state=getsockopt(udp_sock, SOL_SOCKET, SO_TYPE, 
	(void*)&sock_type, &optlen);
	
	if(state)
		error_handling("getsockopt() error!");
	printf("Socket type two: %d \n", sock_type);
	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}