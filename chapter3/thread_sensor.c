#include <stdio.h>

#include <wiringPi.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <softTone.h>


#define SW 5
#define LED 1
#define CDS 0

#define SPKR 6
#define TOTAL 32

#define G 391 //솔
#define A 440 //라
#define E 329 //미
#define F 349 //파
#define D 294 //레
#define C 261 //도
              
int notes2[] = { C, C, G, G, A, A, G, 0,  // "Twinkle twinkle little star"
                F, F, E, E, D, D, C, 0,  // "How I wonder what you are"
                G, G, F, F, E, E, D, 0,  // "Up above the world so high"
                G, G, F, F, E, E, D, 0}; // "Like a diamond in the sky"


void *switchMusic(void *arg)
{
    pinMode(SW,INPUT);
    int cnt=0;
    int i;
    softToneCreate(SPKR);
    while(1) {
        if(digitalRead(SW)==LOW) {
            for(i=0;i<TOTAL;i++){
                if(digitalRead(SW)==HIGH){
                    softToneWrite(SPKR,0);
                    break;
                }
                softToneWrite(SPKR, notes2[i]);
                delay(200);
            }
        }else {
            softToneWrite(SPKR,0);
        }
        delay(100);
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


int main(int argc, char **argv)
{
    wiringPiSetup();

    pthread_t musicThread, ledThread;


    //스레드 생성 
    pthread_create(&musicThread, NULL, switchMusic,NULL);
    pthread_create(&ledThread,NULL,lightLed,NULL);

    //스레드 종료 대기
    pthread_join(musicThread,NULL);
    pthread_join(ledThread,NULL);

    return 0;
}
