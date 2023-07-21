#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

void * read(void * arg);
void * accu(void * arg);

static sem_t sem_one;
static sem_t sem_two;
static int num;

int main(int argc, char *argv[])
{
	pthread_t id_t1, id_t2;

	//创建2个信号量，
	//sem_one的初始值为0
	//sem_two的初始值为1
	sem_init(&sem_one, 0, 0);
	sem_init(&sem_two, 0, 1);

	pthread_create(&id_t1, NULL, read, NULL);
	pthread_create(&id_t2, NULL, accu, NULL);

	pthread_join(id_t1, NULL);
	pthread_join(id_t2, NULL);

	//销毁信号量
	sem_destroy(&sem_one);
	sem_destroy(&sem_two);

	return 0;
}

void * read(void * arg)
{
	int i;
	for(i=0; i<5; i++)
	{
		fputs("Input num: ", stdout);

		//在进入临界区前使用 sem_wait 来将信号量减为 0（信号量初始值为 1）
		sem_wait(&sem_two);
		scanf("%d", &num);
		//临界区执行结束后用 sem_post 来将信号量sem_one加到 1。
		//此时信号量sem_two为0
		sem_post(&sem_one);
	}

	return NULL;	
}
//利用信号量变量sem_two调用 wait函数和 post函数。这是为了防止在调
//用 accu 函数的线程取得数据的情况前，调用 read函数的线程覆盖原num的值
void * accu(void * arg)
{
	int sum=0, i;
	for(i=0; i<5; i++)
	{
		//信号量的值不能小于0 ，
		//因此， 在信号量为0的情况下调用 sem_wait函数时，调用函数的线程将
		//进入阻塞状态(因为函数未返回)
		//但是
		//read()函数执行完毕后，信号量sem_one为1
		//可以进入临界区，sem_one变为0
		sem_wait(&sem_one);
		sum+=num;
		//临界区执行结束后用 sem_post 来将信号量sem_two加到 1。
		//此时信号量sem_one为0
		sem_post(&sem_two);
	}

	printf("Result: %d \n", sum);

	return NULL;
}
//利用信号量变量sem_one调用 wait和 post函数。这是为了防止调用 read 函
//数的线程写入新值前， accu 函数取走(再取走旧值)数据。