/**
 * @Filename: cli.c
 * @Brief   :
 * @Author  : zhang.meiqing 
 * @Version : 1.0.0
 * @Date    : 07/31/2017 03:53:51 PM
 */

#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
#include <sys/socket.h>  
#include <sys/types.h>  
#include <unistd.h>  
#include <arpa/inet.h>  
#include <netinet/in.h>  
#include <errno.h>  
#include <strings.h>  
#include <ctype.h>  
  
  
char * host_name = "10.3.22.166";  
//char * host_name = "127.0.0.1";  
int port = 8888;  
  
int main(int argc , char * argv[])  
{  
    char buf[8192];  
    //char message[256];  
    int socket_descriptor;  
    struct sockaddr_in pin;  
    char * str ="This is just for a test string";  

#if 0
    if(argc < 2)  
    {  
        printf("we will send a default test string.\n");  

    }  
    else  
    {  
        str = argv[1];  
        if(argc == 3)  
        {  
            host_name = argv[2];  
        }  
    }  
#endif

    bzero(&pin,sizeof(pin));  
    pin.sin_family = AF_INET;  
    inet_pton(AF_INET,host_name,&pin.sin_addr);  
    pin.sin_port = htons(port);  
    if((socket_descriptor =  socket(AF_INET,SOCK_STREAM,0)) == -1) {//与服务器端相对应  
        perror("error opening socket \n");  
        exit(1);  
    }  
    printf("start socket connect\n");
    if(connect(socket_descriptor,(struct sockaddr * )&pin,sizeof(pin)) == -1) {
        perror("error connecting to socket \n");  
        exit(1);  
    }  

    printf("socket connect ok\n");
    do {
        if (write(socket_descriptor,str,strlen(str)+1) <= 0) {
            printf("socket write failed\n");
            break;
        }
    
        sleep(1);
    } while (1);
#if 0        
    printf("sending message %s to server ..\n",str);  
    if( write(socket_descriptor,str,strlen(str)+1) == -1 )//将string写入socket描述符中  
    {  
        perror("error in send \n");  
        exit(1);  
    }  

    printf("..sent message ...wait for message..\n");  
    if( read(socket_descriptor,buf,8192) == -1 )//从socket描述符中将8192个字符读到buf中  
    {  
        perror("error in receiving response from server \n");  
        exit(1);  
    }  

    printf("\nResponse from server:\n\n%s\n",buf);  
#endif        
    close(socket_descriptor);  
    return 1;  
}  
