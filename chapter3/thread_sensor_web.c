#include <stdio.h>
#include <wiringPi.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <softTone.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>



#define SW 5
#define LED 1
#define CDS 0

#define SPKR 6
#define TOTAL 32
#define MOTOR 2

#define G 391 //솔
#define A 440 //라
#define E 329 //미
#define F 349 //파
#define D 294 //레
#define C 261 //도

static int is_run=1;
int notes2[] = { C, C, G, G, A, A, G, 0,  // "Twinkle twinkle little star"
                F, F, E, E, D, D, C, 0,  // "How I wonder what you are"
                G, G, F, F, E, E, D, 0,  // "Up above the world so high"
                G, G, F, F, E, E, D, 0}; // "Like a diamond in the sky"

//스위치를 누를대마다 알람 상태 변화
void *switchMusic(void *arg)
{
    pinMode(SW,INPUT);
    int cnt=0;
    int i;
    softToneCreate(SPKR);
    while(1) {
        if(digitalRead(SW)==LOW) {
            delay(10);
            cnt++;
            printf("cnt : %d\n",cnt);
            while(digitalRead(SW)==LOW){
            }
            if(cnt%2==1){
                printf("ALALM ON!\n");
                for(i=0;i<TOTAL;i++){
                    if(digitalRead(SW)==LOW){
                        while(digitalRead(SW)==LOW){}
                        softToneWrite(SPKR,0);
                        printf("ALALM OFF!\n");
                        break;

                    }
                    softToneWrite(SPKR,notes2[i]);
                    delay(200);
                }
                cnt=0;
            }else {
                softToneWrite(SPKR,0);
            }

        }
        delay(50);
    }
    return NULL;
}

void *lightLed(void *arg) {
    pinMode(CDS, INPUT);
    pinMode(LED, OUTPUT);

    while (1) {
        int cdsValue = digitalRead(CDS);
        if (cdsValue == LOW) {  // 빛이 어두우면 LED 켜기
            digitalWrite(LED, LOW);
        } else {  // 빛이 밝으면 LED 끄기
            digitalWrite(LED, HIGH);
        }
        delay(100);
    }
    return NULL;
}

void *motor(void *arg){
    int i;
    pinMode(MOTOR,OUTPUT);
    for(i=0;i<10;i++){
        digitalWrite(MOTOR,HIGH);
        delay(500);
        digitalWrite(MOTOR,LOW);
        delay(500);
    }
    return 0;
}


void *webserverFunction(void* arg)
{
    int ssock;
    pthread_t thread;
    struct sockaddr_in servaddr, cliaddr;
    unsigned int len;

    int port(int)(arg);

    ssock=socket(AF_INET, SOCK_STREAM,0);
    if(ssock ==-1){
        perror("socket()");
        exit(1);
    }
    memset(&servaddr, 0,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);
    if(bind(ssock, (struct sockaddr *)&servaddr, sizeof(servaddr))==-1){
        perror("bind()");
        exit(1);
    }
    if(listen(ssock,10)==-1){
        perror("listen()");
        exit(1);
    }

    while(is_run){
        char mesg[BUFSIZ];
        int csock;

        len=sizeof(cliaddr);
        csock = accept(ssock, (struct sockaddr *)&cliaddr,&len);
        inet_ntop(AF_INET, &cliaddr.sin_addr, mesg, BUFSIZ);

        printf("Client IP : %s:%d\n",mesg, ntohs(cliaddr.sin_port));

        pthread_create(&thread,NULL, clnt_connection, &csock);

    }
    return 0;

}

int main(int argc, char **argv)
{
    wiringPiSetup();

    pthread_t musicThread, ledThread, motorThread;


    //스레드 생성 
    pthread_create(&musicThread, NULL, switchMusic,NULL);
    pthread_create(&ledThread,NULL,lightLed,NULL);
//    pthread_create(&motorThread,NULL,motor,NULL);

    //스레드 종료 대기
    pthread_join(musicThread,NULL);
    pthread_join(ledThread,NULL);
 //   pthread_join(motorThread,NULL);

    if(argc !=2) {
        printf("usage : %s <port>\n",argv[0]);
        return -1;
    }
    return 0;
}
