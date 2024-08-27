#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>

ing listDir(char *arg)
{
	DIR *pdir;
	struct dirent *dirt;
	struct stat statBuf;
	struct passwd *username;
	struct group *groupname;
	struct tm *t;
	int i=0,count=0;
	char *dirName[255], buf[255],permission[11],mtime[20];

	memset(dirName,0,sizeof(dirName));
	memset(&dirt,0,sizeof(dirt));
	memset(&statBuf,0,sizeof(statBuf));
	if((pdir=opendir(arg)) <=0 ) {
		perror ("opendir");
		return -1;
	}
	
	chdir(arg);
	getcwd(buf,255);
	pritnf("\n%s : Directory\n",arg);

