#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
	int status;
	pid_t pid=fork();
	
	if(pid==0)
	{
		return 3;//第9行创建的子进程将在此通过向 
		//父进程传递其return语句的返回值以终止子进程
	}
	else
	{
		printf("Child PID: %d \n", pid);
		pid=fork();

		if(pid==0)
		{
			exit(7);//第19行创建的子进程将在此通过向 
		//父进程传递其exit参数值以终止子进程
		}
		else
		{
			printf("Child PID: %d \n", pid);

			//子进程是否正常终止，是则返回终止的子进程的PID
			//如果没有子进程终止则会阻塞，一直等到有终止的子进程为止
			//status变量用于接收子进程的状态信息
			wait(&status);
			
			//通过宏WIFEXITED获取status中记录的子进程是否正常终止的信息
			//正常终止则返回true
			if(WIFEXITED(status))
				printf("Child send one: %d \n", WEXITSTATUS(status));
			//通过宏WEXITSTATUS获取status中记录的子进程的返回值信息
			//返回子进程的返回值

			wait(&status);

			if(WIFEXITED(status))
				printf("Child send two: %d \n", WEXITSTATUS(status));

			//用于暂停父进程，此时可用ps -au 查看子进程的状态
			sleep(5);     // Sleep 5 sec.
		}
	}
	return 0;
}