#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>

#define BUF_SIZE 100
void error_handling(char *buf);

int main(int argc, char *argv[])
{
	if(argc!=2) {
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}

	int serv_sock, clnt_sock;
	struct sockaddr_in serv_adr, clnt_adr;
	serv_sock=socket(PF_INET, SOCK_STREAM, 0);
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family=AF_INET;
	serv_adr.sin_addr.s_addr=htonl(INADDR_ANY);
	serv_adr.sin_port=htons(atoi(argv[1]));
	
	if(bind(serv_sock, (struct sockaddr*) &serv_adr, sizeof(serv_adr))==-1)
		error_handling("bind() error");
	if(listen(serv_sock, 5)==-1)
		error_handling("listen() error");

	fd_set reads, cpy_reads;
	FD_ZERO(&reads);
	//向reads结构体中注册文件描述符serv_sock的信息
	//即注册服务器端套接字
	FD_SET(serv_sock, &reads);
	//文件和套接字创建时才会被分配文件描述符。
	//它们的文件描述符会从 3 开始按序递增，因此fd_max为3
	fd_max=serv_sock;

	struct timeval timeout;
	socklen_t adr_sz;
	int fd_max, str_len, fd_num, i;
	char buf[BUF_SIZE];
	while(1)
	{
		cpy_reads=reads;
		timeout.tv_sec=5;
		timeout.tv_usec=5000;

		//监视文件描述符数量为fd_max+1，
		//将所有关注"是否存在待读取数据"的文件描述符已注册到fd_set型变量，
		//要传递其地址值
		//不关注"是否可传输无阻塞数据"的文件描述符
		//不关注"是否发生异常"的文件描述符
		if((fd_num=select(fd_max+1, &cpy_reads, 0, 0, &timeout))==-1)
			break;
		
		if(fd_num==0)
			continue;

		//select()函数返回大于等于1时进入循环
		//即发生关注的事件
		for(i=0; i<fd_max+1; i++)
		{
			//用FD_ISSET宏筛选发生状态变化的文件描述符
			if(FD_ISSET(i, &cpy_reads))
			{
				//发生状态变化时，首先验证服务器端套接字中是否有变化。
				//如果是服务器端套接字的变化，将受理连接请求。
				if(i==serv_sock)     // connection request!
				{
					adr_sz=sizeof(clnt_adr);

					//accept()会返回创建的客户端套接字文件描述符
					clnt_sock=
					accept(serv_sock, (struct sockaddr*)&clnt_adr,
					 &adr_sz);

					//使用宏来向结构体中注册文件描述符clnt_sock的信息
					FD_SET(clnt_sock, &reads);

					if(fd_max<clnt_sock)
						fd_max=clnt_sock;

					printf("connected client: %d \n", clnt_sock);
				}
				else    // read message!
				{
					//发生变化的套接字并非服务器端套接字时，
					//即有要接受的数据时执行else语句,
					//但此时需要确认接收的数据是字符串还是代表
					//断开连接的 EOF。
					str_len = read(i, buf, BUF_SIZE);
					if (str_len == 0) // close request!
					{
						//接收的数据为 EOF时需要关闭套接字，
						//并从 reads 中删除相应信息
						FD_CLR(i, &reads);
						close(i);
						printf("closed client: %d \n", i);
					}
					else
					{
						//接收的数据为字符串时，执行回声服务。
						write(i, buf, str_len); // echo!
					}
				}
			}
		}
	}

	close(serv_sock);
	return 0;
}

void error_handling(char *buf)
{
	fputs(buf, stderr);
	fputc('\n', stderr);
	exit(1);
}