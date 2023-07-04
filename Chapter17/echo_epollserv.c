#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/epoll.h>

#define BUF_SIZE 100
#define EPOLL_SIZE 50
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

	//创建指向epoll_event结构体的指针ep_events
	struct epoll_event *ep_events;
	//创建epoll_event结构布局的结构变量event
	struct epoll_event event;
	//创建epfd以存储文件描述符
	int epfd, event_cnt;

	//1.创建保存epoll文件描述符的空间，“epoll例程”
	//此处的EPOLL_SIZE可以用0代替
	//epoll_create()返回创建的epoll例程的文件描述符
	epfd=epoll_create(EPOLL_SIZE);
	//指向epoll_event结构体的指针ep_events
	//指向申请分配的内存空间
	ep_events=malloc(sizeof(struct epoll_event)*EPOLL_SIZE);

	//注册关注的事件
	event.events=EPOLLIN;
	//注册文件描述符
	event.data.fd=serv_sock;	
	//2.向epfd代表的例程中注册文件描述符serv_sock，
	//主要目的是监视EPOLLIN(需要读取数据的情况)的事件。
	epoll_ctl(epfd, EPOLL_CTL_ADD, serv_sock, &event);

	socklen_t adr_sz;
	int str_len, i;
	char buf[BUF_SIZE];
	while(1)
	{
		//3.等待文件描述符的变化
		//在ep_events指向的内存空间中保存epfd代表的例程中
		//发生事件的文件描述符的集合
		//返回发生事件的文件描述符数量
		event_cnt=epoll_wait(epfd, ep_events, EPOLL_SIZE, -1);

		if(event_cnt==-1)
		{
			puts("epoll_wait() error");
			break;
		}

		//循环遍历发生事件的文件描述符
		for(i=0; i<event_cnt; i++)
		{
			//若发生变化的是服务器端文件描述符
			if(ep_events[i].data.fd==serv_sock)
			{
				adr_sz=sizeof(clnt_adr);
				clnt_sock=
					accept(serv_sock, (struct sockaddr*)&clnt_adr, &adr_sz);

				//注册关注的事件
				event.events=EPOLLIN;
				//注册客户端文件描述符
				event.data.fd=clnt_sock;

				epoll_ctl(epfd, EPOLL_CTL_ADD, clnt_sock, &event);
				printf("connected client: %d \n", clnt_sock);
			}
			else    // read message!
			{
				//发生变化的套接字并非服务器端套接字时，
				//即有要接受的数据时执行else语句,
				//但此时需要确认接收的数据是字符串还是代表
				//断开连接的 EOF 。
				str_len=read(ep_events[i].data.fd, buf, BUF_SIZE);

				//若传递的是EOF
				if(str_len==0)    // close request!
				{
					//向epfd代表的例程中删除文件描述符
					epoll_ctl(
						epfd, EPOLL_CTL_DEL, ep_events[i].data.fd, NULL);
					close(ep_events[i].data.fd);
					printf("closed client: %d \n", ep_events[i].data.fd);
				}
				else
				{
					write(ep_events[i].data.fd, buf, str_len);    // echo!
				}
	
			}
		}
	}

	close(serv_sock);

	//关闭该epoll例程
	close(epfd);

	return 0;
}

void error_handling(char *buf)
{
	fputs(buf, stderr);
	fputc('\n', stderr);
	exit(1);
}
