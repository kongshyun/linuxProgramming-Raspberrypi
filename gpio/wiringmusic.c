#include <wiringPi.h>
#include <softTone.h>

#define SPKR 6
#define TOTAL 32

#define G 391 //솔
#define A 440 //라
#define E 329 //미
#define F 349 //파 
#define D 294 //레
#define C 261 //도
int notes[] = { G, G, A, A, G, G,E,E,\
    G,G,E,E,D,D,D,0,\
    G,G,A,A,G,G,E,E,\
    G,E,D,E,C,C,C,0};
int notes2[] = { C, C, G, G, A, A, G, 0,  // "Twinkle twinkle little star"
                F, F, E, E, D, D, C, 0,  // "How I wonder what you are"
                G, G, F, F, E, E, D, 0,  // "Up above the world so high"
                G, G, F, F, E, E, D, 0}; // "Like a diamond in the sky"
int musicPlay()
{
    int i;
    softToneCreate(SPKR);
    for(i=0;i<TOTAL;i++){
        softToneWrite(SPKR,notes2[i]);
        delay(100);
    }
    return 0;
}

int main()
{
    wiringPiSetup();
    musicPlay();
    return 0;
}
