#include "server.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>

#define TCP_PORT 5100 /*서버의 포트 번호*/

int main(int argc, char **argv)
{
    int ssock;
//    int csock;
    socklen_t clen;  /*소켓 디스크립터 정의 */
    pid_t childpid; // 자식 프로세스 ID
    int n;
    struct sockaddr_in servaddr, cliaddr;/*주소 구조체 정의 */

    char mesg[BUFSIZ]; 
    int status;
    /*서버 소켓 생성*/
    if((ssock = socket(AF_INET, SOCK_STREAM,0))<0) {
        perror("socket()");
        return -1;
    }
    
    printf("Server Socket is created.! \n");

    /*주소 구조체에 주소 지정 */
    memset(&servaddr, 0,sizeof(servaddr));
    servaddr.sin_family=AF_INET;
    servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
    servaddr.sin_port = htons(TCP_PORT);

    if(bind(ssock,(struct sockaddr *)&servaddr, sizeof(servaddr))<0) {
        perror("bind()");
        return -1;
    }
    /*대기하는 클라이언트 숫자 설정. */
    if(listen(ssock,10)==0){ // 수정 
        perror("listen()");
        return -1;
    }
    clen=sizeof(cliaddr);
    do {
        int n;
        int csock = accept(ssock,(struct sockaddr *)&cliaddr, &clen);
        /*클라이언트를 문자열로 변경 */
        
        inet_ntop(AF_INET, &cliaddr.sin_addr,mesg,BUFSIZ);
        printf("Client is connected : %s\n",mesg);
        /*자식프로세스 생성 */
        if((childpid=fork())<0){
            perror("Error");
        
        }else if (childpid==0){

            close(ssock); //서버 소켓 ID를 닫는다. 
            do{
                memset(mesg,0,BUFSIZ);
                if ((n=read(csock, mesg, BUFSIZ))<=0)
                     perror("read()");
                     printf("Received data : %s",mesg);


                /*클라이언트로 buf에 있는 문자열 전송 */
                if(write(csock, mesg, n) <=0)
                    perror("write()");
            } while(strncmp(mesg,"q",1));
    
        close(csock);/* 클라이언트 소켓을 닫음*/
        exit(0);
        
        
        }
        wait(&status); //세기님 추가 
        close(csock);
    }while(strncmp(mesg,"q",1));

    close(ssock);/*서버 소켓을 닫음 */
    return 0;
}
