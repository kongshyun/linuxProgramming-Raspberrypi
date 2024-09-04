/*
 *병렬처리할수 있도록 한다. 
 *select() 함수 사용 
 *
 *
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>


#define SERVER_PORT 5100 /*서버의 포트번호 */

int main(int argc, char **argv)
{
    int ssock;
    socklen_t clen;
    int n;
    struct sockaddr_in servaddr, cliaddr;
    char mesg[BUFSIZ];

    fd_set readfd; /*select함수를 위한 자료형 */
    int maxfd, client_index, start_index;
    int client_fd[5]={0}; /*client의 소켓 FD배열 */


    /*서버 소켓 디스크립터를 연다 */
    //IPv4, TCP 프로토콜
    if((ssock = socket(AF_INET, SOCK_STREAM,0))<0) {
        perror("socket()");
        return -1;
    }


    /*운영체제에 서비스 등록 */
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family=AF_INET;
    servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERVER_PORT);

    /*소켓에 서버의 IP주소와 포트를 바인딩 */
    if(bind(ssock, (struct sockaddr*)&servaddr, sizeof(servaddr))<0)
    {
        perror("bind()");
        return -1;
    }
    /*클라이언트 연결 대기 */
    if(listen(ssock,8)<0){
        perror("listen()");
        return -1;
    }
    
    FD_ZERO(&readfd); /*fd_set자료형ㅇ르 모두 0으로 초기화 */
    maxfd=ssock;    /*현재 최대 파일 디스크립터의 번호는 서버 소켓의 디스크립터 */
    client_index=0;

    do {
        FD_SET(ssock, &readfd); /*서버소켓을 감시할파일디스크립터로 설정 */
        /*클라이언트의 시작부터 마지막 주소까지 fd_set 자료형에 설정 */
        for(start_index=0; start_index < client_index;start_index++) {
            FD_SET(client_fd[start_index],&readfd);
            if(client_fd[start_index]>maxfd)
                maxfd=client_fd[start_index];
        }
    
        maxfd=maxfd+1;
        select(maxfd, &readfd, NULL, NULL, NULL);
        if(FD_ISSET(ssock,&readfd)){
            clen = sizeof(struct sockaddr_in);
            int csock = accept(ssock, (struct sockaddr*)&cliaddr,&clen);
            if(csock<0){
                perror("accept()");
                return -1;
            } else {
                inet_ntop(AF_INET, &cliaddr.sin_addr,mesg, BUFSIZ);
                printf("Client is connected : %s\n",mesg);

                FD_SET(csock,&readfd);
                client_fd[client_index] = csock;
                client_index++;
                continue;
            }
            if(client_index == 5) break;
        }

        for(start_index = 0; start_index < client_index; start_index++){
            if(FD_ISSET(client_fd[start_index], &readfd)) {
                memset(mesg,0,sizeof(mesg));

                if((n=read(client_fd[start_index],mesg, sizeof(mesg)))>0){
                    printf("Received data : %s",mesg);
                    write(client_fd[start_index],mesg,n);
//                    close(client_fd[start_index]);

//                    FD_CLR(client_fd[start_index],&readfd);
//                    client_index--;
                }
            }
        }

    }while(strncmp(mesg,"q",1));
    close(ssock);
    return 0;

}
