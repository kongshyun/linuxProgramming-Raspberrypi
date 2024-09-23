#include <wiringPi.h>
#include <softTone.h>

#define SPKR 6

// 음계 주파수 정의 (도, 레, 미, 파, 솔, 라, 시)
#define G 391 // 솔
#define A 440 // 라
#define E 329 // 미
#define F 349 // 파
#define D 294 // 레
#define C 261 // 도

// 음을 나타내는 구조체
typedef struct {
    int frequency;  // 음의 주파수 (0이면 쉼표)
    int duration;   // 음 길이 (밀리초 단위)
} Note;

// "Twinkle Twinkle Little Star" 멜로디 (음계와 길이)
Note melody[] = {
    { C, 500 }, { C, 500 }, { G, 500 }, { G, 500 }, { A, 500 }, { A, 500 }, { G, 1000 }, // Twinkle twinkle little star
    { F, 500 }, { F, 500 }, { E, 500 }, { E, 500 }, { D, 500 }, { D, 500 }, { C, 1000 }, // How I wonder what you are
    { G, 500 }, { G, 500 }, { F, 500 }, { F, 500 }, { E, 500 }, { E, 500 }, { D, 1000 }, // Up above the world so high
    { G, 500 }, { G, 500 }, { F, 500 }, { F, 500 }, { E, 500 }, { E, 500 }, { D, 1000 }  // Like a diamond in the sky
};

int melodyLength = sizeof(melody) / sizeof(Note);  // 멜로디 배열의 길이 계산

// 음 재생 함수
void playTone(Note note) {
    if (note.frequency == 0) {
        delay(note.duration);  // 주파수가 0이면 쉼표 (음 없음)
    } else {
        softToneWrite(SPKR, note.frequency);  // 음 재생
        delay(note.duration);  // 음의 길이만큼 유지
    }
}

void playMelody() {
    for (int i = 0; i < melodyLength; i++) {
        playTone(melody[i]);  // 각 음을 차례대로 재생
        delay(50);  // 음과 음 사이의 짧은 딜레이 (쉼)
    }
}

int main() {
    wiringPiSetup();
    softToneCreate(SPKR);  // 피에조 버저 핀에 소프트 톤 생성
    playMelody();  // 멜로디 재생
    return 0;
}

