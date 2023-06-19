#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
void error_handling(char *message);

int main(int argc, char *argv[])
{
	if(argc!=2) {
		printf("Usage : %s <addr>\n", argv[0]);
		exit(1);
	}
	
	struct hostent *host;
	host=gethostbyname(argv[1]);
	if(!host)
		error_handling("gethost... error");

	printf("Official name: %s \n", host->h_name);
	
	int i;
	for(i=0; host->h_aliases[i]; i++)
		printf("Aliases %d: %s \n", i+1, host->h_aliases[i]);
	
	printf("Address type: %s \n", 
		(host->h_addrtype==AF_INET)?"AF_INET":"AF_INET6");

	for(i=0; host->h_addr_list[i]; i++)
		printf("IP addr %d: %s \n", i+1,
		inet_ntoa(*(struct in_addr*)host->h_addr_list[i]));
		//将h_addr_list的元素即指向字符串的指针强制类型转换成
		//指向in_addr结构布局的指针，并解引用后得到in_addr结构体
		//可以作为inet_ntoa()函数的是实参。
	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
