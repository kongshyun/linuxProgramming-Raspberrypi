#include <stdio.h>
#include <unistd.h>
#include <wiringPi.h>
#include <stdlib.h>
int number[10][4]=
{
    {0,0,0,0},
    {0,0,0,1},
    {0,0,1,0},
    {0,0,1,1},
    {0,1,0,0},
    {0,1,0,1},
    {0,1,1,0},
    {0,1,1,1},
    {1,0,0,0},
    {1,0,0,1}
};

int gpio[4]={4,1,16,15};//a,b,c,d,e,f,g

int ledControl(int num){
    for(int i=0;i<4;i++){
        pinMode(gpio[i],OUTPUT);

    }
    for(int i=0;i<4;i++){
        digitalWrite(gpio[i],number[num][i]?HIGH:LOW);
    }
    getchar();
    for(int i=0;i<4;i++){
        digitalWrite(gpio[i],HIGH);
    }
    return 0;
}

int main(int argc, char **argv)
{
    int gno;
    if(argc<2){
        printf("Usage : %s GPIO_NO\n",argv[0]);
        return -1;
    }
    gno=atoi(argv[1]);
    wiringPiSetup();
    ledControl(gno);
    return 0;


}
