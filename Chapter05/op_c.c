#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024
#define DATA_SIZE 4
void error_handling(char *message);

int main(int argc, char *argv[])
{
	int sock;
	struct sockaddr_in serv_adr;

	if(argc!=3) {
		printf("Usage : %s <IP> <port>\n", argv[0]);
		exit(1);
	}
	
	sock=socket(PF_INET, SOCK_STREAM, 0);   
	if(sock==-1)
		error_handling("socket() error");
	
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family=AF_INET;
	serv_adr.sin_addr.s_addr=inet_addr(argv[1]);
	serv_adr.sin_port=htons(atoi(argv[2]));
	
	if(connect(sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr))==-1)
		error_handling("connect() error!");
	else
		puts("Connected...........");
	
	
    //思路：
    //显示的提示语句operand count: 不是服务端发出的，
    //可以是客户端用某些表达式实现将字符串信息显示在显示器上

    // printf("Operand count: ");
    fputs("Operand count: ", stdout);

    int op_count;
    scanf("%d",&op_count);

    //为了支持在同一个数组中传送多个不同的数据类型的数据
    //应该把数组元素声明为char类型，
    //具体存数据时，根据数据类型来确定指针类型，同时设置好指针指向的位置
    char opmsg[BUF_SIZE];
    //int类型强制类型转换为char型
    // opmsg[0] = char(op_count);  //***注1***这样写编译错误
    opmsg[0]=(char)op_count;

    for(int i=0;i<op_count;i++)
    {
        printf("Operand %d: ",i+1);

        //******将4字节整型数保存到字符数组中,需要将其转换成int指针类型
        //int指针类型保存的是个int值的地址
        scanf("%d",(int* )&opmsg[i*DATA_SIZE+1]); 
        //从opmsg[1]开始存，数据占4个字节，即到opmsg[4]
        //下次从opmsg[5]开始存       
    }

    fputs("Operator: ",stdout);

    //消耗掉换行符(放在fputs()语句之前也可以)
    getc(stdin);    
    //或写为：
    // fgetc(stdin);

    // getchar();//此函数不行，程序不会显示Operator:字样

    //能否不处理？？？
    //***注2***
    //若不处理，则程序会卡在显示Operator:字样之前 

    //***理解***
    //scanf()在使用%d或%s时，会跳过空白开始读取第1个非空白字符，
    //%d是遇到非数字字符就停止，%s是字段末尾或遇到空白就停止，
    //但使用%c时，若一开始就输入\n则会读取\n

    //存运算符
    scanf("%c",&opmsg[op_count*DATA_SIZE+1]);

    write(sock,opmsg,op_count*DATA_SIZE+2);

    int result;
    read(sock,&result,sizeof(result));

    printf("Operation result: %d \n",result);

    close(sock);
    
	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}