#include <stdio.h>
#include <unistd.h>
#include <signal.h>

//定义信号处理函数(信号处理器Handler)
void timeout(int sig)
{
	if(sig==SIGALRM)
		puts("Time out!");

	alarm(2);//为了每隔2秒重复产生SIGALRM信号，在信号处理器中
}			 //调用alarm函数

//定义信号处理函数(信号处理器Handler)
void keycontrol(int sig)
{
	if(sig==SIGINT)
		puts("CTRL+C pressed");
}

int main(int argc, char *argv[])
{
	int i;

	//注册信号SIGALRM、SIGINT
	//及相应处理器
	signal(SIGALRM, timeout);
	signal(SIGINT, keycontrol);

	//预约2秒后发生SIGALRM信号
	alarm(2);

	for(i=0; i<3; i++)
	{
		puts("wait...");

		sleep(10);
	}
	
	return 0;
}