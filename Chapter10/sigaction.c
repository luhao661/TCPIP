#include <stdio.h>
#include <unistd.h>
#include <signal.h>

void timeout(int sig)
{
	if(sig==SIGALRM)
		puts("Time out!");
		
	alarm(2);	
}

int main(int argc, char *argv[])
{
	int i;

	//为了注册信号处理函数，声明sigaction结构体变量
	struct sigaction act;
	//在sa_handler成员中保存函数指针值
	act.sa_handler=timeout;
	//调用sigemptyset()将sa_mask成员的所有位初始化为0
	sigemptyset(&act.sa_mask);
	//将sa_flags成员赋值为0
	act.sa_flags=0;

	//注册SIGALRM信号的信号处理函数
	sigaction(SIGALRM, &act, 0);

	//调用alarm函数预约2秒后发生SIGALRM信号
	alarm(2);

	for(i=0; i<3; i++)
	{
		puts("wait...");
		sleep(10);
	}
	return 0;
}