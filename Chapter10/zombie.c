#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	pid_t pid=fork();
	
	if(pid==0)     // if Child Process
	{
		puts("Hi I'am a child process");
	}
	else
	{
		//***注***
		//在父进程中，fork()返回的值是子进程的PID
		printf("Child Process ID: %d \n", pid);
		sleep(5);     // Sleep 5 sec.
		//父进程暂停5秒，因为如果父进程终止，
		//那么处于僵尸状态的子进程将同时被销毁，
		//因此延缓父进程的执行以验证僵尸进程
	}

	if(pid==0)
		puts("End child process");
	else
		puts("End parent process");
	return 0;
}
