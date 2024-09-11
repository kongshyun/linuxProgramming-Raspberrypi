#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define TCP_PORT 5100

//메세지 구조체 정의
typedef struct {
    char type[10];  //메세지 타입("LOGIN", "LOGOUT", "MSG")
    char username[50];  // 사용자 이름
    char content[256];  // 메시지 내용
} Message;

int main(int argc, char **argv)
{
    int ssock;
    struct sockaddr_in servaddr;
    Message msg; //메시지 구조체 
    char username[50]; // 사용자 이름 입력받을 변수 
    char mesg[BUFSIZ];

    if(argc <2){
        printf("Usage : %s IP_ADRESS\n",argv[0]);     
        return -1;
    }

    /*소켓을 생성*/
    if((ssock=socket(AF_INET,SOCK_STREAM,0))<0){
        perror("socket()");
        return -1;
    }
    /*소켓이 접속할 주소 지정 */
    memset(&servaddr,0,sizeof(servaddr));
    servaddr.sin_family = AF_INET;


    /*문자열을 네트워크 주소로 변경 */
    inet_pton(AF_INET, argv[1],&(servaddr.sin_addr.s_addr));
    servaddr.sin_port = htons(TCP_PORT);

    /*지정한 주소로 접속*/
    if(connect(ssock,(struct sockaddr *)&servaddr, sizeof(servaddr))<0) {
        perror("connect()");
        return -1;
    }
    
    printf("Enter your username: ");
    fgets(username, sizeof(username),stdin);
    username[strcspn(username,"\n")]=0; //개행 문제 제거 

    strcpy(msg.type,"LOGIN");
    strcpy(msg.username, username);
    if(send(ssock,&msg,sizeof(msg),0)<=0){
        perror("send()");
        return -1;
    }

    while(1){
        printf("Enter message (or 'q' to quit): ");
        fgets(msg.content,sizeof(msg.content),stdin);
        msg.content[strcspn(msg.content,"\n")]=0; //개행 문자 제거
        
        // 종료 조건
        if (strcmp(msg.content, "q") == 0) {
            break;
        }

        // 일반 메시지 전송
        strcpy(msg.type, "MSG");
        if (send(ssock, &msg, sizeof(msg), 0) <= 0) {
            perror("send()");
            return -1;
        }

        // 서버로부터 응답 수신
        if (recv(ssock, &msg, sizeof(msg), 0) <= 0) {
            perror("recv()");
            return -1;
        }

        // 받은 메시지 출력
        printf("[%s]: %s\n", msg.username, msg.content);
        

    }

    //소켓 종료 
    close(ssock);
    return 0;
}
