#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/select.h>

#define TCP_PORT 5100

int main(int argc, char** argv) {
    int sockfd;
    struct sockaddr_in servaddr;

    fd_set readfds;
    char mesg[BUFSIZ];

    

    if(argc<2){
        const char* str="Usage : client IP_ADDRESS\n";
        write(2,str,strlen(str));
    }
    

    sockfd = socket(AF_INET,SOCK_STREAM,0); // 소켓을 생성 
    if(sockfd <0) {
        perror("socket()");
        return -1;
    }

    memset(&servaddr,0,sizeof(servaddr));
    servaddr.sin_family=AF_INET;

    inet_pton(AF_INET, argv[1], &(servaddr.sin_addr.s_addr));
    servaddr.sin_port = htons(TCP_PORT);

    /*지정한 주소로 접속 */
    if(connect(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr))<0) {
        perror("connect()");
        return -1;
    }

    /* @ FD가 do-while문 안에 있어야 하는 이유??
     * select()가 호출되기 전에 항상 FD_ZERO, FD_SET이 실행되어야 하므 
     * */
    
    do{
        
        FD_ZERO(&readfds);
        FD_SET(0, &readfds);
        FD_SET(sockfd,&readfds);
       
        /*select되면 멈춰있음. */
        select(sockfd+1, &readfds, NULL, NULL, NULL);
        
        //키보드로부터 값이 오면 서버로 보내기 
        if (FD_ISSET(0, &readfds)) {
            int size = read(0, mesg, BUFSIZ);
            write(sockfd, mesg, size);
        }

        //서버로부터 값이 오면 출력 
        else if(FD_ISSET(sockfd, &readfds)) {
            int size = read(sockfd, mesg, BUFSIZ);
            if(size==0) break;/*서버가 닫히면 0이 반환됨.*/
            rite(1,mesg,size);
        }
    }
    while(strncmp(mesg,"q",1));

    close(sockfd);
    return 0;
}

