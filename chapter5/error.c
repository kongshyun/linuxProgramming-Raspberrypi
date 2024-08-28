/*write함수에서 1과 2 차이*/
#include <fcntl.h>
#include <unistd.h>

int main() 
{
	write(1, "Hello World\n",12);
//	write(2, "Hello world\n",12);
	return 0;
}
