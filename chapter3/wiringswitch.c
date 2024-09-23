#include <wiringPi.h>
#include <stdio.h>

#define SW 5
#define LED 1

int switchControl()
{
    int i;
    pinMode(SW, INPUT);
    pinMode(LED,OUTPUT);

    digitalWrite(LED,LOW);
    int cnt=0;
    for(;;){
        if(digitalRead(SW) == LOW){
            delay(10);
            cnt++;
        
            printf("cnt : %d\n",cnt);
            while(digitalRead(SW)==LOW){
            }
            if(cnt%2==1){
                digitalWrite(LED,HIGH);//켜진 상태 유지 
            }
            else if(cnt%2==0){
                digitalWrite(LED,LOW);
            }
        }
        delay(10);
    }
    return 0;
}

int main(int argc, char **argv)
{
    wiringPiSetup();
    switchControl();
    return 0;
}
