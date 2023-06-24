#include <stdio.h>
#include <unistd.h>
#define BUF_SIZE 30

int main(int argc, char *argv[])
{
	int fds[2];
	char str[]="Who are you?";
	char buf[BUF_SIZE];
	pid_t pid;
	
	//调用pipe函数创建管道，fds数组中保存
	//用于I/O的文件描述符
	pipe(fds);

	pid=fork();
	//理解：
	//子进程和父进程将同时拥有通过fork()函数调用获取的2个文件描述符。
	//注意复制的并非管道，而是用于管道I/O的文件描述符。至此，父子进程
	//同时拥有I/O文件描述符。

	if(pid==0)
	{
		//子进程向管道传递字符串
		write(fds[1], str, sizeof(str));
	}
	else
	{
		//父进程从管道接收字符串
		read(fds[0], buf, BUF_SIZE);
		puts(buf);
	}
	
	return 0;
}