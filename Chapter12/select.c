#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/select.h>

#define BUF_SIZE 30

int main(int argc, char *argv[])
{
	//创建fd_set结构体将要监视的文件描述符集中到一起
	//以监视这些套接字
	fd_set reads;

	//创建timeval结构布局的结构变量timeout
	//以作为select函数的第五个实参
	struct timeval timeout;

	//使用宏来完成对结构体中所有位都设置为0的操作
	FD_ZERO(&reads);
	//使用宏来向结构体中注册文件描述符fd0的信息
	//将文件描述符0对应的位设置为1，即
	//需要监视标准输入的变化
	FD_SET(0, &reads); // 0 is standard input(console)

	//***注***
	//这是为了设置select函数的超时而添加的。但不能在此时设置超时。
	//因为调用 select 函数后，结构体 timeval 的成员tv_sec和tv-usec
	//的值将被替换为超时前剩余时间。因此，调用 select函数前，每次都
	//需要初始化timeval结构体变量。
	/*
	timeout.tv_sec=5;
	timeout.tv_usec=5000;
	*/

	fd_set temps;
	int result, str_len;
	char buf[BUF_SIZE];
	while(1)
	{
		//temps暂存reads结构变量的内容
		//将准备好的fd_set变量reads的内容复制到temps变量，
		//因为之前讲过，调用 select 函数后，除发生变化的文件描述符对应位外，
		//剩下的所有位将初始化为 0。因此，为了记住初始值，
		//必须经过这种复制过程。这是使用 select函数的通用方法，
		//希望各位牢记。
		temps=reads;

		//应该在此处设置超时，将初始化timeval 结构体的代码插入循环后，
		//每次调用 select 函数前都会初始化新值。
		timeout.tv_sec=5;
		timeout.tv_usec=0;

		//监视文件描述符数量为1，
		//将所有关注"是否存在待读取数据"的文件描述符注册到fd_set型变量，
		//并传递其地址值
		//不关注"是否可传输无阻塞数据"的文件描述符
		//不关注"是否发生异常"的文件描述符
		result=select(1, &temps, 0, 0, &timeout);
		//调用 select 函数。如果有控制台输入数据，则返回大子0的整数;
		//如果没有输入数据而引发超时，则返回 0 。

		//select函数返回大于0 的值时运行的区域。验证发生变化的文件描述符
		//是否为标准输入。若是，则从标准输入读取数据并向控制台输出。
		if(result==-1)
		{
			puts("select() error!");
			break;
		}
		else if(result==0)
		{
			puts("Time-out!");
		}
		else 
		{
			if(FD_ISSET(0, &temps)) 
			{
				str_len=read(0, buf, BUF_SIZE);
				buf[str_len]=0;
				printf("message from console: %s", buf);
			}
		}
	}
	return 0;
}