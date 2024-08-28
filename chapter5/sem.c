/*교수님 이 주신 세마포어 코드 (0828)*/

#include <stdio.h>
#include <unistd.h>
#include <sys/sem.h>
#include <signal.h>
#include <string.h>

int cnt=0;
static int semid;
static void sigHandler(int);

void p()
{
	struct sembuf pbuf;
	pbuf.sem_num	=0;
	pbuf.sem_op		=-1;
	pbuf.sem_flg	= SEM_UNDO;
	if(semop(semid, &pbuf,1)==-1)
		perror("p : semop()");
}


void v()
{
	struct sembuf vbuf;
	vbuf.sem_num =0;
	vbuf.sem_op =1;
	vbuf.sem_flg=SEM_UNDO;
	if(semop(semid, &vbuf,1) == -1)
		perror("v= : semop()");
}

static void sigHandler(int signo)
{
	if(signo == SIGINT) {
		printf("SIGINT is catched : %d\n",signo);
		v();
	}else {
		fprintf(stderr, "Catched signal: %s\n", strsignal(signo));
	}

}

int main(int argc, char **argv) 
{
	union semun {
		int		val;
		struct semid_ds *buf;
		unsigned short int *array;
	} arg;
	if(signal(SIGINT,sigHandler)==SIG_ERR) {
		perror("signal() : SIGINT");
		return -1;
	}
	if((semid=semget(IPC_PRIVATE,1,IPC_CREAT | 0666)) == -1) {
		perror("semget()");
		return -1;
	}
	//arg.val=1;
	arg.val=8;
	if(semctl(semid,0,SETVAL,arg)==-1) {
		perror("semctl() : SETVAL");
		return -1;
	}
	/*
	while(1) {
		if(cnt >=8) {
			cnt--;
			p();
			printf("decrease : %d\n",cnt);
			break;
		}else {
			cnt ++;
			v();
			printf("increase : %d\n", cnt);
			usleep(100);
		}
	}
*/
	p();
	p();
	p();
	p();
	p();
	p();
	p();
	p();
	printf("%s %d\n", __FUNCTION__,__LINE__);
	v();
	printf("%s %d\n", __FUNCTION__, __LINE__);
	if(semctl(semid,0,IPC_RMID,arg) ==-1) {
		perror("semctl() : IPC_RMID");
		return -1;
	}
	return 0;
}
		
