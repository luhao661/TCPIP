#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024
#define DATA_SIZE 4
int cal(int, int* , char);
void error_handling(char* message);

int main(int argc, char** argv)
{
    if(argc!=2)
    {
        printf("Usage : %s <port>\n",argv[0]);
    }

    //服务器端和客户端的套接字文件描述符
    int server_sock;
    int client_sock;

    //创建套接字，用文件描述符server_sock表示
    server_sock = socket(PF_INET, SOCK_STREAM, 0);
    if(server_sock==-1)
    {
        error_handling("socket() error");
    }

    //创建服务器端的sockaddr_in结构布局的结构体变量，以便存储IP和端口，
    //最后再变为地址，用指向sockaddr结构体的指针读取
    struct sockaddr_in serv_addr;

    memset(&serv_addr,0,sizeof(serv_addr));
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    serv_addr.sin_port=htons(atoi(argv[1]));

    //给套接字分配IP地址和端口
    if(bind(server_sock,(struct sockaddr*)&serv_addr,sizeof(serv_addr))
    ==-1)
    {
        error_handling("bind() error");
    }

    //调用listen()函数，服务器端进入等待连接请求状态，创建连接请求等待队列
    if(listen(server_sock,5)==-1)
    {
        error_handling("listen() error");
    }

    //创建客户端的sockaddr_in结构布局的结构体变量，
    //最后再变为地址，用指向sockaddr结构体的指针存地址，
    //以便写入客户端的IP和端口
    struct sockaddr_in clnt_addr;
	socklen_t clnt_addr_size = sizeof(clnt_addr);

    //accept()函数会受理连接请求等待队列中待处理的客户端连接请求
    client_sock=accept(server_sock,(struct sockaddr*)&clnt_addr,&clnt_addr_size);
    if(client_sock==-1)
    {
        error_handling("accept() error");
    }

    //以下写法不可行，因为没有换行符的话不会刷新缓冲区
    // char str1="Operand count: ";
    // write(client_sock, str1, sizeof(str1));

 
	printf("Client connected! \n");
    
    //读取操作数个数
    //(还剩下op_count*DATA_SIZE+1个字节要读)

    //***注1***
    //错误写法：
    // int op_count;
    int op_count=0;//***此处一定要将高位都置为0，因为传进来的是char类型的值，只有8位
	read(client_sock,&op_count,1);

    //测试：
    //操作数个数输入2，为什么显示4197378？？？
    // printf("%d \n",op_count);
    //上面的注已经给出解答

    //读取操作数
    //***注2***为了解决TCP粘包/拆包问题
    //编写循环，来调用read函数直到读取了后面的op_count*DATA_SIZE+1个字节为止
    int recv,recv_cnt;
    recv=recv_cnt=0;
    char opinfo[BUF_SIZE];

    while(recv<(op_count*DATA_SIZE+1))
    {
        //***理解***
        //read()的第三个参数是指要接收的最大字节数
        //返回值表示系统实际所读取的字符数量，
        //（不读取垃圾值，若有换行符，则也会读取）
        //https://blog.csdn.net/luke_sanjayzzzhong/article/details/100855623
        recv_cnt=read(client_sock,&opinfo,BUF_SIZE-1);
        recv+=recv_cnt;
    }
   
    //***理解***
    //原本是char数组首元素的地址，现在强制类型转换为int元素的地址
    //所以要写为(int* )opinfo

    int result=cal(op_count,(int* )opinfo,opinfo[recv-1]);
    //传操作数个数，存操作数的数组，运算符

    // ***注3***错误写法：
    // int result=cal(op_count,(int* )opinfo,opinfo[op_count*DATA_SIZE+1]);

    //测试：
    //以2个操作数的输入为例，前者输出8，后者输出9，
    //考虑到元素opinfo[0]先前已被读取，所以前者正确
    // printf("op_count*DATA_SIZE+1=%d \n",op_count*DATA_SIZE+1);
    // printf("recv-1=%d \n",recv-1);

    //虽然result是int类型，但write()是以字节为单位传输的，
    //所以要强制类型转换为char元素的地址，然后长度是4个字节
    write(client_sock,(char* )&result,sizeof(result));

    close(client_sock);
    close(server_sock);

    return 0;
}
int cal(int count, int* p, char op)
{
    int result=p[0];
    int i=0;

    switch(op)
    {
        case '+':
        for(i=1;i<count;i++)
        result+=p[i];
        break;

        case '-':
        for(i=1;i<count;i++)
        result-=p[i];
        break;

        case '*':
        for(i=1;i<count;i++)
        result*=p[i];
        break;

        default :
        error_handling("error Operator!");
    }

    return result;
}

void error_handling(char* message)
{
    fputs(message,stderr);
    fputc('\n',stderr);
    exit(1);
}
