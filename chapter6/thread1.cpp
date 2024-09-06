#include <iostream>
#include <thread>

using namespace std;

void call_from_thread(int tid){
    cout << "스레드 실행 : "<<tid << endl;
    std::this_thread::sleep_for(std::chrono::nanoseconds(tid*20));
}

int main(){
    thread t[10];

    thread(call_from_thread,0).detach();
    for(int i=0;i<10;i++){
        t[i]=thread(call_from_thread,i);
    }

    cout << "메인함수 시작"<<endl;

    for(int i=0;i<10;i++){
        if(t[i].joinable())
            t[i].join();
    }

    return 0;

}
