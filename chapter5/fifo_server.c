#include <stdio.h>
#include <fcntl.h>
#include <unistd.h> /*read(), close(), unlink() 사용가능*/
#include <sys/stat.h>

#define FIFOFILE "fifo"

int main(int argc,char **argv)
{
	int n,fd;
	char buf[BUFSIZ];

	unlink(FIFOFILE);
	if(mkfifo(FIFOFILE, 0666) <0 ) { //새로운 FIFO파일 생성
		perror("mkfifo()");
		return -1;
	}
	if((fd=open(FIFOFILE, O_RDONLY))<0) {
		perror("open()");
		return -1;
	}

	while((n=read(fd, buf,sizeof(buf))) >0) 
		printf("%s", buf);

	close(fd);
	return 0;
}
