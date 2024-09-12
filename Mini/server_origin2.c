#include "server.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>// O_NONBLOCK 플래그를 사용하기 위한 헤더 파일

#define TCP_PORT 5100 /*서버의 포트 번호*/
#define MAX_CLIENTS 10

//클라이언트 정보 구조체
typedef struct{
    int sockfd;         //클라이언트 소켓 파일 디스크립터
    char username[50];  //클라이언트 이름
    pid_t pid; // 클라이언트 프로세스 ID 
}Client;

Client clients[MAX_CLIENTS]; //클라이언트 리스트
int client_count =0;
int running_children=0; // 실행중인 자식 프로세스 수

//메시지 구조체
typedef struct{
    char type[10];  //메시지 타입("LOGIN","MSG")
    char username[50];//사용자이름
    char content[256];//메시지 내용 
}Message;


//int pipefd[2]; //[0]읽기 끝, [1]쓰기 끝 
int pipe1[MAX_CLIENTS][2]; //자식 -> 부모 
int pipe2[MAX_CLIENTS][2]; //부모 -> 자식


void setup_pipes(){
    for(int i=0;i<MAX_CLIENTS;i++){
        if(pipe(pipe1[i])== -1 || pipe(pipe2[i])==-1){
            perror("pipe()");
            exit(1);
        }
    }
}

void set_nonblocking_pipe(int pipefd[2]){
    int flags;
    flags = fcntl(pipefd[0],F_GETFL,0);
    if(flags ==-1 ||fcntl(pipefd[0],F_SETFL,flags | O_NONBLOCK)== -1){
        perror("fcntl() for read end of pipe");
        exit(1);
    }
    flags = fcntl(pipefd[1],F_GETFL,0);
    if(flags == -1 || fcntl(pipefd[1],F_SETFL, flags | O_NONBLOCK)==-1){
        perror("fcntl() for write end of pipe");
        exit(1);
    }
    
}

void setup_nonblocking_pipes(){
    for(int i=0;i<MAX_CLIENTS;i++){
        set_nonblocking_pipe(pipe1[i]);
        set_nonblocking_pipe(pipe2[i]);
    }
}


// SIGUSR1 핸들러 함수
void sigusr1_handler(int sig) {
    Message msg;
    // 자식 프로세스는 파이프에서 메시지를 읽음
    int process_index = getpid();  // 프로세스 ID로 클라이언트 인덱스 결정 (또는 다른 방법으로 인덱스 확인)
    if (read(pipe2[process_index][0], &msg, sizeof(msg)) > 0) {
        printf(" → Received broadcast from parent [%s]: %s\n", msg.username, msg.content);
    }
}

void broadcast_message(Message *msg) {
    for (int i = 0; i < client_count; i++) {
        // 파이프에 메시지 쓰기
        if (write(pipe2[i][1], msg, sizeof(*msg)) == -1) {
            perror("write()");
        }
        // SIGUSR1 신호를 자식 프로세스에게 전송
        kill(clients[i].pid, SIGUSR1);
    }
}

//자식 프로세스가 종료되었을때 처리
void handle_sigchld(int signum){
    int status;
    pid_t pid;

    while((pid=waitpid(-1, &status, WNOHANG))>0){
        running_children--; // 자식프로세스 수 감소 
        printf("Child process PID %d : 종료, (남은자식: %d)\n", pid,running_children);
/*
        //남은 자식이 없으면 부모도 종료 
        if(running_children ==0){
            printf("모든 자식프로세스 종료. 서버를 종료합니다.\n");
//            close(pipefd[0]);
//            close(pipefd[1]);
            exit(0);
        }*/
    }
}


//파이프에서 메시지를 읽고 처리하는 함수  (논블로킹모드 -> 반복적으로 )
void handle_pipe_read() {
    
    Message msg;
    int n;

    //파이프에서 메시지를 반복적으로 읽어처리 
    while(1){
        for(int i=0;i<client_count;i++){
            n=read(pipe1[i][0],&msg,sizeof(msg)); //자식 -> 부모 파이프
        
            if(n>0){
                printf("[PARENT] Received message from child: [%s]: %s\n",msg.username, msg.content);
                broadcast_message(&msg);
        
            }else if(n==-1){
                perror("read() from pipe1");
            }
        }
    }
}




int main(int argc, char **argv)
{
    //시그널  핸들러 설정
    signal(SIGCHLD,handle_sigchld);
    
    int ssock,csock;
    Message msg; //메시지 구조체 
    pid_t pid;
    struct sockaddr_in servaddr, cliaddr;/*주소 구조체 정의 */
    socklen_t clen=sizeof(cliaddr);  /*소켓 디스크립터 정의 */

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
    if(listen(ssock,MAX_CLIENTS)<0){
        perror("listen()");
        return -1;
    }

    printf("서버가 포트 %d에서 대기 중...\n",TCP_PORT);

    setup_pipes();
    setup_nonblocking_pipes();
    do {
        //클라이언트 연결 수락 
        csock = accept(ssock,(struct sockaddr *)&cliaddr, &clen);
        if(csock<0){
            perror("accept()");
            continue;
        }

        //자식프로세스 생성 
        if((pid=fork())<0){
            perror("fork()");
        
        }else if (pid==0){
            close(ssock); //자식 프로세스에서는 서버 소켓을 닫음
            
            int process_index = client_count;

            //로그인 및 메시지 처리
            //
            do{
                memset(&msg, 0,sizeof(msg)); //메시지 구조체 초기화
                int n= recv(csock, &msg,sizeof(msg),0);
                

                if (n<=0){
                    if(n==0){
                        printf("Client disconnected.\n");
                    }else{
                        perror("recv()");
                    }
                    break;
                }
                if(strcmp(msg.content,"q")==0){
                    //클라이언트가 'q'
                    printf("%s has logout(q) the chat.\n",msg.username);
                    break;
                }

                if(strcmp(msg.type,"LOGIN")==0){
                    //클라이언트 로그인 처리
                    printf("▷ [%s] 님이 로그인\n",msg.username);
                    clients[client_count].sockfd = csock;
                    clients[client_count].pid=getpid(); //프로세스ID 
                    strcpy(clients[client_count].username, msg.username);
                    client_count++;
                }
                if(strcmp(msg.type, "MSG")==0){
                    printf("[CHILD] Sending message to parent: [%s]: %s\n", msg.username, msg.content);
                    //부모 프로세스로 메시지 전송
                    if(write(pipe1[process_index][1],&msg,sizeof(msg))==-1){
                        perror("write() to parent");
                    }
                    //부모로부터 메시지 수신
                    if(read(pipe2[process_index][0],&msg,sizeof(msg))>0){
                        printf(" → Recieved from parent [%s] : %s\n",msg.username,msg.content);
                    }
                }   
            }while(1); //종료 조건이 발생할 때까지 루프

            close(csock);/* 클라이언트 소켓을 닫음*/
            exit(0);     //자식 프로세스 종료
        }
        running_children++;  // 실행 중인 자식 프로세스 수 증가
        printf("New client connected. Current running children: %d\n", running_children);
        close(csock); //부모프로세스에서 클라이언트 소켓 닫기
        
        //부모는 파이프에서 메시지 읽기
    }while(1); //서버가 종료되기 전가지 클라이언트연결수락
    

    handle_pipe_read();
    close(ssock);/*서버 소켓을 닫음 */
    return 0;
}
