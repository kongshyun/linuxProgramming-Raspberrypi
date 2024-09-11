#include "server.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#define TCP_PORT 5100 /*서버의 포트 번호*/
#define MAX_CLIENTS 10

//클라이언트 정보 구조체
typedef struct{
    int sockfd;         //클라이언트 소켓 파일 디스크립터
    char username[50];  //클라이언트 이름
}Client;

Client clients[MAX_CLIENTS]; //클라이언트 리스트
int client_count =0;

//메시지 구조체
typedef struct{
    char type[10];  //메시지 타입("LOGIN","MSG")
    char username[50];//사용자이름
    char content[256];//메시지 내용 
}Message;

int pipefd[2]; //[0]읽기 끝, [1]쓰기 끝 


void broadcast_message(Message *msg, int sender_sock) {
    for (int i = 0; i < client_count; i++) {
        if (clients[i].sockfd != sender_sock) {
            send(clients[i].sockfd, msg, sizeof(*msg), 0);
        }
    }
}

void handle_pipe_read() {
    
    Message msg;
    read(pipefd[0],&msg,sizeof(msg)); //파이프에서 메시지를 읽기.
    printf("[PARENT PROCESS] Received message from child: [%s]: %s\n",msg.username, msg.content);
    broadcast_message(&msg,-1); //모든클라이언트에게 브로드캐스트

}

//시그널 
void sigusr1_handler(int signum){
    printf("Received SIGUSR1 from child process.\n");
}



int main(int argc, char **argv)
{
    //SIGUSR1 핸들러 설정
    signal(SIGUSR1,sigusr1_handler);
    
    int ssock,csock;
    Message msg; //메시지 구조체 
    pid_t pid;
    int n;
    struct sockaddr_in servaddr, cliaddr;/*주소 구조체 정의 */
    socklen_t clen=sizeof(cliaddr);  /*소켓 디스크립터 정의 */

    /*서버 소켓 생성*/
    if((ssock = socket(AF_INET, SOCK_STREAM,0))<0) {
        perror("socket()");
        return -1;
    }
        
    printf("Server Socket is created.! \n");

    //파이프 생성
    if (pipe(pipefd)==-1){
        perror("pipe()");
        return -1;
    }


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
    if(listen(ssock,MAX_CLIENTS)<0){
        perror("listen()");
        return -1;
    }

    printf("Server is listening on port %d\n",TCP_PORT);

    do {
        //클라이언트 연결 수락 
        csock = accept(ssock,(struct sockaddr *)&cliaddr, &clen);
        if(csock<0){
            perror("accept()");
            continue;
        }

        //자식프로세스 생성 
        if((pid=fork())<0){
            perror("Error");
        
        }else if (pid==0){
            close(ssock); //자식 프로세스에서는 서버 소켓을 닫음
            //로그인 및 메시지 처리
            do{
                memset(&msg, 0,sizeof(msg)); //메시지 구조체 초기화
                n= recv(csock, &msg,sizeof(msg),0);
                
                if (n<=0){
                    if(n==0){
                        printf("Client disconnected.\n");
                    }else{
                        perror("recv()");
                    }
                    break;
                }
                if(strcmp(msg.type,"LOGIN")==0){
                    //클라이언트 로그인 처리
                    printf("%s logged in.\n",msg.username);
                    clients[client_count].sockfd = csock;
                    strcpy(clients[client_count].username, msg.username);
                    client_count++;
                } else if(strcmp(msg.type, "MSG")==0){
                    write(pipefd[1],&msg,sizeof(msg)); //파이프로 부모에게 메시지 전송
                    //서버가 받은 메시지를 클라이언트에게 다시 돌려보냄.
                    if(send(csock,&msg,sizeof(msg),0)<=0){
                        perror("send()");
                        break;
                    }
                }   
            }while(1); //종료 조건이 발생할 때까지 루프

            close(csock);/* 클라이언트 소켓을 닫음*/
            exit(0);     //자식 프로세스 종료
        }
        handle_pipe_read();
        close(csock); //부모프로세스에서 클라이언트 소켓 닫기

    }while(1); //서버가 종료되기 전가지 클라이언트연결수락
    

    close(ssock);/*서버 소켓을 닫음 */
    return 0;
}
