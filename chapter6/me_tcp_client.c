#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>

#define TCP_PORT 5100

int main(int argc, char **argv)
{
    int ssock;
    struct sockaddr_in servaddr;
    char mesg[BUFSIZ];
//----------
    pid_t pid;
    int pfd[2];
    int status;
//----------

    if(argc <2){
        printf("Usage : %s IP_ADRESS\n",argv[0]);     
    }
    /*소켓을 생성*/
    if((ssock=socket(AF_INET,SOCK_STREAM,0))<0){
        perror("socket()");
        return -1;
    }
    /*소켓이 접속할 주소 지정 */
    memset(&servaddr,0,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    
    if(pipe(pfd)<0){
        perror("pipe()");
        return -1;
    }
    if((pid=fork())<0){
        perror("fork()");
        return -1;
    }
    else if(pid==0)

    do{
        /*문자열을 네트워크 주소로 변경 */
        inet_pton(AF_INET, argv[1],&(servaddr.sin_addr.s_addr));
        servaddr.sin_port = htons(TCP_PORT);
        if((pid=fork())<0){
            perror("Error");
        }else if(pid==0){
        }


    }while(strncmp(mesg,"q",1));
    


    /*지정한 주소로 접속*/
    if(connect(ssock,(struct sockaddr *)&servaddr, sizeof(servaddr))<0) {
        perror("connect()");
        return -1;
    }

    fgets(mesg, BUFSIZ, stdin);
    if(send (ssock, mesg, BUFSIZ, MSG_DONTWAIT)<=0) { /*데이터를 소켓에 쏨.*/
        perror("send()");
        return -1;
    }

    shutdown(ssock,SHUT_WR); // 출력 소켓을 닫는다. 

    /*데이터를 소켓으로부터 읽음.*/
    memset(mesg, 0, BUFSIZ);
    if (recv(ssock,mesg,BUFSIZ,0) <=0){
        perror("recv()");
        return -1;
    }
    
    shutdown(ssock,SHUT_RD); // 입력소켓을닫는다.
    /*받은 문자열을 화면에 출력 */
    printf("Received data : %s ", mesg);


    close(ssock);

    return 0;
}
