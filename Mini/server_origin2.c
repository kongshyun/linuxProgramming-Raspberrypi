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
                        printf("Client 연결 종료.\n");
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
