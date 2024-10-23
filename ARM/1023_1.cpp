#include <iostream>

using namespace std;

int main(int argc, char * argv[])
{
    cout << "argc : " << argc << endl;
    while(*argv) {
        cout << *(argv++) << endl;
        goto top;
    }
    return 0;
}
