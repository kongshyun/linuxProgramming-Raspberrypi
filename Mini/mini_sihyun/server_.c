
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>

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

void broadcast_message(Message *msg,int sender_pid);
void child_sigusr1_handler(int sig);
void parent_sigusr1_handler (int sig);
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

// 자식 프로세스에서 메시지 처리 (클라이언트로부터 메시지 수신 후)
void handle_client_message(int process_index, int csock) {
//    printf("핸들클라이언트 메시지!!");
    Message msg;

    signal(SIGUSR1,child_sigusr1_handler);
    while(1){
        memset(&msg, 0, sizeof(msg)); // 메시지 초기화
        int n = recv(csock, &msg, sizeof(msg), 0);
        
        if (n <= 0) {
            if (n == 0) {
                //printf("Client 연결 종료.\n");
            } else {
                perror("recv()");
            }
            break;
        }

        if (strcmp(msg.content, "q") == 0) {
            printf("%s has logout(q) the chat.\n", msg.username);
            break;
        }

        if (strcmp(msg.type, "LOGIN") == 0) {
            printf(" --- [%s] 님이 입장하셨습니다.---\n", msg.username);
            clients[process_index].sockfd = csock;
            clients[process_index].pid = getpid(); // 프로세스 ID 저장
            strcpy(clients[process_index].username, msg.username);
//            client_count++;
        }

        if (strcmp(msg.type, "MSG") == 0) {
            printf("[%s]메시지: %s\n", msg.username, msg.content);
            // 부모 프로세스로 메시지 전송
            if (write(pipe1[process_index][1], &msg, sizeof(msg)) == -1) {
                perror("write() to parent");
            }
            // 부모에게 SIGUSR1 신호 보내기
            kill(getppid(), SIGUSR1);
       }

    }

    close(csock);
    exit(0); // 자식 프로세스 종료
}

//부모 -> 자식 에게 메세지 전달 
void broadcast_message(Message *msg,int sender_pid) {
//    printf("broadcast!!\n");
    for (int i = 0; i < client_count; i++) {
        // 파이프에 메시지 쓰기
        if(clients[i].pid == sender_pid){
            continue;
        }
        if (write(pipe2[i][1], msg, sizeof(*msg)) == -1) {
            perror("write()");
        }
        // SIGUSR1 신호를 자식 프로세스에게 전송
        if(kill(clients[i].pid, SIGUSR1)==-1){
            perror("kill() error");
        }else {
            printf("SIGUSR1 sent to child process (PID: %d)\n", clients[i].pid);
        }
    }
}

void parent_sigusr1_handler(int sig) {
    Message msg;

    // 부모 프로세스일 경우 자식 프로세스로부터 메시지를 읽음
    for (int i = 0; i < client_count; i++) {
        int n = read(pipe1[i][0], &msg, sizeof(msg));
        if (n > 0) {
            //printf("[PARENT] Received message from child [%s]: %s\n", msg.username, msg.content);
            // 받은 메시지를 다른 자식 프로세스에 브로드캐스트
            broadcast_message(&msg,clients[i].pid);
        }
    }
}

void child_sigusr1_handler(int sig) {
    Message msg;
    int process_index = -1;

    // 현재 자식 프로세스의 인덱스 확인
    for (int i = 0; i < client_count; i++) {
        if (clients[i].pid == getpid()) {
            process_index = i;
            break;
        }
    }

    if (process_index != -1) {
        printf("[CHILD] Received SIGUSR1 in child process (PID: %d)\n", getpid());  // 신호 수신 확인
        // 부모 프로세스로부터 메시지를 읽음
        if (read(pipe2[process_index][0], &msg, sizeof(msg)) > 0) {
            printf("[CHILD] Received broadcast from parent: [%s]: %s\n", msg.username, msg.content);
            // 받은 메시지를 해당 클라이언트 소켓으로 전송
            if (send(clients[process_index].sockfd, &msg, sizeof(msg), 0) <= 0) {
                perror("send() to client");
            } else {
                printf("자식 -> 클라이언트 보냄.");
            }
        }else {
            perror("read() 파이프 실패!");
        }
    }
}

//자식 프로세스가 종료되었을때 처리
void handle_sigchld(int signum){
    int status;
    pid_t pid;

    while((pid=waitpid(-1, &status, WNOHANG))>0){
        running_children--; // 자식프로세스 수 감소 
        printf("  >> PID %d 님이 퇴장하셨습니다. (현재인원: %d)\n", pid,running_children);
    }
}
int main(int argc, char **argv)
{
    //시그널  핸들러 설정
    signal(SIGCHLD,handle_sigchld);
    signal(SIGUSR1,parent_sigusr1_handler);
    
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
        
    printf("  '***채팅방에 오신걸 환영합니다***' \n");

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

    printf("------채팅방에 아무도 없습니다------\n");

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
            //printf("자식프로세스 생성!! PID : %d \n",getpid());
            close(ssock); //자식 프로세스에서는 서버 소켓을 닫음
            int process_index = client_count;
            handle_client_message(process_index,csock); 
            
            close(csock);/* 클라이언트 소켓을 닫음*/
            exit(0);     //자식 프로세스 종료
        }
        //else if(pid>0){
        //    printf("부모프로세스!!\n");
        // }
        running_children++;  // 실행 중인 자식 프로세스 수 증가
        client_count++;
        printf("새로운 멤버가 채팅방에 입장했습니다. 현재인원 : %d\n", running_children);
        close(csock); //부모프로세스에서 클라이언트 소켓 닫기
        
        //부모는 파이프에서 메시지 읽기
    }while(1); //서버가 종료되기 전가지 클라이언트연결수락
    
    close(ssock);/*서버 소켓을 닫음 */
    return 0;
}
