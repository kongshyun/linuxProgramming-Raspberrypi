#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char **argv) 
{
	pid_t pid;
	int pfd[2];
	char line[BUFSIZ];
	int status;

	if(pipe(pfd) <0) {
		perror("pipe()");
		return -1;
	}
	printf("%d %d\n",pfd[0], pfd[1]);    //3 4
	if((pid=fork()) <0) {
		perror("fork()");
		return -1;
	} else if(pid ==0) {
		close(pfd[0]);
//		dup2(pfd[1],1);
//		execl("/bin/date","date",NULL);
//		write(1,"Hello World\n",12);
		write(pfd[1], "Hello World\n", 12);
		close(pfd[1]);
		_exit(217);
	}else {
		close(pfd[1]);
		if(read(pfd[0], line, BUFSIZ) <0 ) {
			perror("read()");
			return -1;
		}
		printf("%s", line);
		close(pfd[0]);
		waitpid(pid,&status,0);
	}
	return 0;
}
