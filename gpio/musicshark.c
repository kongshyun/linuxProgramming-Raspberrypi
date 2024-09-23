#include <wiringPi.h>
#include <softTone.h>

#define SPKR 6  // 피에조 버저 연결 핀

// 음계 주파수 정의 (도, 레, 미, 파, 솔, 라, 시)
#define C 261 // 도
#define D 294 // 레
#define E 329 // 미
#define F 349 // 파
#define G 391 // 솔
#define A 440 // 라
#define B 493 // 시
#define P 523 // 피

// 음을 나타내는 구조체
typedef struct {
    int frequency;  // 음의 주파수 (0이면 쉼표)
    int duration;   // 음 길이 (밀리초 단위)
} Note;

// "아기상어" 첫 구절 멜로디 (음계와 길이)
Note melody[] = {
    { D, 250 }, { E, 250 }, { G, 250 }, { G, 250 }, { G, 250 }, { G, 250 }, { A, 250 }, { G, 500 }, // 레 미 솔 솔 솔 솔 라 솔
    { G, 250 }, { G, 250 }, { D, 250 }, { E, 250 }, { G, 250 }, { G, 250 }, { G, 250 }, { G, 500 }, // 솔 솔 레 미 솔 솔 솔 솔
    { G, 250 }, { G, 250 }, { A, 250 }, { G, 250 }, { A, 250 }, { G, 250 }, { G, 250 }, { G, 250 }, // 솔 솔 라 솔 라 솔 솔 솔
    { G, 250 }, { G, 250 }, { P, 500 }, { P, 500 } // 솔 솔 피 피
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
        delay(100);  // 음과 음 사이의 짧은 딜레이 (쉼)
    }
}

int main() {
    wiringPiSetup();
    softToneCreate(SPKR);  // 피에조 버저 핀에 소프트 톤 생성
    playMelody();  // 멜로디 재생
    return 0;
}

