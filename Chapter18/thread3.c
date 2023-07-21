#include <stdio.h>
#include <pthread.h>

void * thread_summation(void * arg); 

int sum=0;

int main(int argc, char *argv[])
{
	pthread_t id_t1, id_t2;
	int range1[]={1, 5};
	int range2[]={6, 10};
	
	pthread_create(&id_t1, NULL, thread_summation, (void *)range1);
	pthread_create(&id_t2, NULL, thread_summation, (void *)range2);

	//main函数将等待id_t1的值对应的线程的终止。
	//***注***
	//第二个实参为NULL，则表示不保存thread_summation()
	//返回的内存空间的地址。
	pthread_join(id_t1, NULL);
	pthread_join(id_t2, NULL);

	printf("result: %d \n", sum);

	return 0;
}

void * thread_summation(void * arg) 
{
	int start=((int*)arg)[0];
	int end=((int*)arg)[1];

	//***注***
	//2个线程直接访问全局变量sum
	//因为2个线程共享全局变量的数据区
	while(start<=end)
	{
		sum+=start;
		start++;
	}

	return NULL;
}
