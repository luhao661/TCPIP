#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#define NUM_THREAD	100

void * thread_inc(void * arg);
void * thread_des(void * arg);

long long num=0;
//创建一个全局的pthread_mutex_t类型的变量
//以保存互斥量的值。之所以声明全局变量是因为thread_inc函数和
//thread_des函数都需要访问互斥量
pthread_mutex_t mutex;

int main(int argc, char *argv[]) 
{
	pthread_t thread_id[NUM_THREAD];
	int i;
	
	//创建互斥量(互斥锁)
	pthread_mutex_init(&mutex, NULL);

	for(i=0; i<NUM_THREAD; i++)
	{
		if(i%2)
			pthread_create(&(thread_id[i]), NULL, thread_inc, NULL);
		else
			pthread_create(&(thread_id[i]), NULL, thread_des, NULL);	
	}	

	for(i=0; i<NUM_THREAD; i++)
		pthread_join(thread_id[i], NULL);

	printf("result: %lld \n", num);

	//销毁互斥量
	pthread_mutex_destroy(&mutex);

	return 0;
}

void * thread_inc(void * arg) 
{
	int i;

	//临界区划分范围较大，可以最大限度减少
	//互斥量lock、unlock函数的调用次数。
	//但会使得其他线程的等待时间增加
	pthread_mutex_lock(&mutex);
	for(i=0; i<50000000; i++)
		num+=1;
	pthread_mutex_unlock(&mutex);
	
	return NULL;
}
void * thread_des(void * arg)
{
	int i;
	for(i=0; i<50000000; i++)
	{
		//临界区划分范围较小，可以减少线程的等待时间
		//但增加了互斥量lock、unlock函数的调用次数
		pthread_mutex_lock(&mutex);
		num-=1;
		pthread_mutex_unlock(&mutex);
	}

	return NULL;
}

