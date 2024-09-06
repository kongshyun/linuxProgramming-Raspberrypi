/*
 * 6장
 * tcp_server.c
 * 혼자 구현해보기!!
 * */
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define TCP_PORT 5100

int main()
{
    int ssock,clen;
    //소켓 디스크립터 정의 
    socklen_t clen;

    struct sockaddr_in servaddr, cliaddr;
    char mesg[BUFSIZ];
    
    if((ssock=socket(AF_INET,SOCK_STREAM,0))<0){
        perror("socket()");
        return -1;
    }

    //소켓 구조체 정의
    memset(&servaddr,0,sizeof(servaddr));
    servaddr.sin_family=AF_INET;
    servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
    servaddr.sin_port = htons(TCP_PORT);
    /*소켓을 운영체제에 등록 해주기*/
    if((bind(ssock,(struct sockaddr *)&servaddr,sizeof(servaddr)))<0){
        perror("bind()");
        return -1;
    }
    
    if(listen(ssock,8)<0){
        perror("listen()");
        return -1;
    }

    clen=sizeof(cliaddr);
    do {
        int n,csock=accept(ssock, (struct sockaddr *)&cliaddr, &clen);

        inet_ntop(AF_INET,&cliaddr.sin_addr,mesg,BUFSIZ);
        printf("Client is connected : %s\n",mesg);

        if((n=read(csock, mesg,BUFSIZ))<0){
            perror("read()");
            return -1;
        }

        if(write(csock,mesg,n)<=0)
            perror("write()");
        close(csock);

    } while (mesg,"q",1);

    csock(ssock);
    return 0;

}


//-------재구현-------
#include <stdio.h>






