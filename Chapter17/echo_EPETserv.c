#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/epoll.h>

//为验证边缘触发的工作方式，将缓冲设置为4字节
#define BUF_SIZE 4
#define EPOLL_SIZE 50
void setnonblockingmode(int fd);
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

	struct epoll_event *ep_events;
	struct epoll_event event;
	int epfd, event_cnt;

	epfd=epoll_create(EPOLL_SIZE);
	ep_events=malloc(sizeof(struct epoll_event)*EPOLL_SIZE);

	//将文件描述符代表的服务器端套接字的属性修改为非阻塞模式
	setnonblockingmode(serv_sock);

	event.events=EPOLLIN;
	event.data.fd=serv_sock;	
	epoll_ctl(epfd, EPOLL_CTL_ADD, serv_sock, &event);

	socklen_t adr_sz;
	int str_len, i;
	char buf[BUF_SIZE];
	while(1)
	{
		event_cnt=epoll_wait(epfd, ep_events, EPOLL_SIZE, -1);

		if(event_cnt==-1)
		{
			puts("epoll_wait() error");
			break;
		}

		//为观察事件发生数而添加的输出字符串的语句
		puts("return epoll_wait");

		for(i=0; i<event_cnt; i++)
		{
			if(ep_events[i].data.fd==serv_sock)
			{
				adr_sz=sizeof(clnt_adr);
				clnt_sock=
					accept(serv_sock, (struct sockaddr*)&clnt_adr, &adr_sz);

				//将文件描述符代表的客户端套接字的属性修改为非阻塞模式
				setnonblockingmode(clnt_sock);

				//注册事件为边缘触发
				event.events=EPOLLIN|EPOLLET;
				event.data.fd=clnt_sock;

				epoll_ctl(epfd, EPOLL_CTL_ADD, clnt_sock, &event);
				printf("connected client: %d \n", clnt_sock);
			}
			else
			{
				//边缘触发方式中，发生事件时需要读取输入缓冲中的所有数据，
				//因此需要循环调用 read 函数
				while(1)
				{
					str_len=read(ep_events[i].data.fd, buf, BUF_SIZE);

					if(str_len==0)    // close request!
					{
						epoll_ctl(
							epfd, EPOLL_CTL_DEL, ep_events[i].data.fd, NULL);
						close(ep_events[i].data.fd);
						printf("closed client: %d \n", ep_events[i].data.fd);
						break;
					}
					else if(str_len<0)
					{
						//read 函数返回 -1 且 errno值为 EAGAIN 时，
						//意味着读取了输入缓冲中的全部数据，因此需要通过
						//break语句跳出while循环
						if(errno==EAGAIN)
							break;
					}
					else
					{
						write(ep_events[i].data.fd, buf, str_len);    // echo!
					}
				}
			}
		}
	}

	close(serv_sock);

	close(epfd);
	
	return 0;
}

void setnonblockingmode(int fd)
{
	int flag=fcntl(fd, F_GETFL, 0);
	fcntl(fd, F_SETFL, flag|O_NONBLOCK);
}
void error_handling(char *buf)
{
	fputs(buf, stderr);
	fputc('\n', stderr);
	exit(1);
}