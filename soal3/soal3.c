#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <syslog.h>
#include <string.h>
#include <pthread.h>

pthread_t tid[20];
char workingdir[150];

int cekfile(const char *path) {
	struct stat path_stat;
	stat(path, &path_stat);
	return S_ISREG(path_stat.st_mode);
}

void *pindah(void *arg) {
	char ext[100], filename[100];
	char *pisah1, *pisah2, *titik[10], *slash[10], path[100];
	int i = 0, j = 0;

	strcpy(path, arg);
	pisah1 = strtok(path, ".");
	while(pisah1 != NULL) {
		titik[i] = pisah1;
		pisah1 = strtok(NULL, ".");
		i++;
	}
	
	printf("|-%s\n", titik[i-1]);
	strcpy(ext, titik[i-1]);

	pisah2 = strtok(path, "/");
	while(pisah2 != NULL) {
		slash[j] = pisah2;
		pisah2 = strtok(NULL, "/");
		j++;
	}

	printf("|--%s\n", slash[j-1]);
	strcpy(filename, slash[j-1]);

	struct dirent *drct;
	DIR *dir = opendir(workingdir), *diropen;
	struct stat filetype;	
	char dir1[150], dir2[150];
	char tujuan[1000], asal[1000];

	if(i >= 2) {
		int flag = 0;
		if(dir == NULL) {
			return 0;
		}
		while((drct = readdir(dir)) != NULL) {
			if(strcmp(drct->d_name, filename) == 0 && filetype.st_mode & S_IFDIR) {
				flag = 1;
				break;
			}
		}
		if(flag == 0) {
			strcpy(dir1, workingdir);
			strcat(dir1, "/");
			strcat(dir1, ext);
			mkdir(dir1, 0777);
		}
	} else {
		strcpy(dir1, workingdir);
		strcat(dir1, "/unknown");
		mkdir(dir1, 0777);
	}

	strcpy(asal, arg);
	strcpy(tujuan, workingdir);
	strcat(tujuan, "/");
	
	if(i == 0) {
		strcat(tujuan, "unknown");
	} else {
		strcat(tujuan, ext);
	}
	
    strcat(tujuan, "/");    
	strcat(tujuan, filename);
	rename(asal, tujuan);
	
	return NULL;
}

int main(int argc, char const *argv[]){
	if(getcwd(workingdir, sizeof(workingdir)) !=  NULL) {
		printf("%s\n", workingdir);
	}

	if(strcmp("-f", argv[1]) == 0) {
		for(int x = 2; x < argc; x++) {
			if(cekfile(argv[x])) {
				pthread_create(&tid[x-2], NULL, pindah, (void *) argv[x]);
			}
		}
		for(int x = 0; x < argc-2; x++) {
			pthread_join(tid[x], NULL);
		}
	} 
	else if(strcmp("-d", argv[1]) == 0 && argc == 3) {
		
	}
	else if(strcmp("*", argv[1]) == 0 && argc == 2) {
        
	} else {
		printf("Invalid Arguments\n");
	}
}
