#include<stdio.h>
#include<string.h>
#include<ctype.h>
#include<pthread.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<dirent.h>

pthread_t tid[100];

char wdasal[1024];

int cekfile(const char *path) {
	struct stat stat_t;
	stat(path, &stat_t);
	return S_ISREG(stat_t.st_mode);
}

void* pindahd(void *arg) {
	char *path;
	char format[100], dirname[100], a[100], b[100], namafile[100];
	path = (char *) arg;
	
	strcpy(a, arg);
	strcpy(b, arg);
	char *split1, *split2, *titik[5], *slash[5];
	int n = 0;
	split1 = strtok(path, ".");
	while(split1 != NULL) {
		titik[n] = split1;
		n++;
		split1 = strtok(NULL, ".");
	}

	if(n != 1) {
		int a;
		for(a = 0; a < strlen(titik[n-1]); a++) {
			format[a] = tolower(titik[n-1][a]);
		}
	}
	else if (n == 1) {
		strcpy(format, "Unknown");
	}

	n = 0;
	split2 = strtok(b, "/");
	while(split2 != NULL) {
		slash[n] = split2;
		n++;
		split2 = strtok(NULL, "/");
	}
	strcpy(namafile, slash[n-1]);

	strcpy(dirname, wdasal);
	strcat(dirname, "/");
	strcat(dirname, format);

	memset(format, 0, 100);
	mkdir(dirname, 0777);

	FILE *psrc, *ptjn;

	psrc = fopen(a, "r");
	strcat(dirname, "/");
	strcat(dirname, namafile);
	ptjn = fopen(dirname, "w");
	if(!psrc) {
		printf("error\n");
	}
	if(!ptjn) {
		printf("error\n");
	}

	int ch;
	
	while ((ch = fgetc(psrc)) != EOF) {
		fputc(ch, ptjn);
	}

	fclose(psrc);
	fclose(ptjn);
	remove(a);

	return NULL;
}

void* pindah(void *arg) {
	char *path;
	char format[100], dirname[100], a[100], b[100], namafile[100];
	
	path = (char *) arg;
	strcpy(a, arg);
	strcpy(b, arg);
	
	char *split1, *split2, *titik[5], *slash[5];
	int n = 0;

	split1 = strtok(path, ".");
	while(split1 != NULL) {
		titik[n] = split1;
		n++;
		split1 = strtok(NULL, ".");
	}
    
	if(n != 1) {
		int a;
		for(a = 0; a < strlen(titik[n-1]); a++) {
		    format[a] = tolower(titik[n-1][a]);
	    }
	}
	else if (n == 1) {
		strcpy(format, "Unknown");
	}

	n = 0;
	split2 = strtok(b, "/");
	while(split2 != NULL) {
		slash[n] = split2;
		n++;
		split2 = strtok(NULL, "/");
	}
	strcpy(namafile, slash[n-1]);

	char wdtujuan[1024];
	getcwd(wdtujuan, sizeof(wdtujuan));
	strcpy(dirname, wdtujuan);
	strcat(dirname, "/");
	strcat(dirname, format);

	// printf("|-%s\n", format);		//mencetak format file
	// printf("|--%s\n", namafile);		//mencetak namafile berserta ekstensinya
	memset(format, 0, 100);
	mkdir(dirname, 0777);

	FILE *psrc, *ptjn;

	psrc = fopen(a, "r");
	strcat(dirname, "/");
	strcat(dirname, namafile);
	ptjn = fopen(dirname, "w");

	if(!psrc) {
		printf("error\n");
	}
	if(!ptjn) {
		printf("error\n");
	}

	int ch;
	while ((ch = fgetc(psrc)) != EOF) {
		fputc(ch, ptjn);
	}

	fclose(psrc);
	fclose(ptjn);
	remove(a);

	return NULL;
}

int main(int argc, char *argv[]) {

	getcwd(wdasal, sizeof(wdasal));

	// if(getcwd(wdasal, sizeof(wdasal)) != NULL) {
	// 	printf("%s\n", wdasal);
	// }

	int i;

	if(strcmp(argv[1], "-f") == 0) {
		for(i = 2; i < argc; i++) {
			if(cekfile(argv[i])) {
				pthread_create(&(tid[i-2]), NULL, pindah, (void *)argv[i]);
			}
		}
		for(i = 0; i < argc - 2; i++) {
			pthread_join(tid[i], NULL);
		}
	}

	else if(strcmp(argv[1], "*") == 0 && argc == 2) {
		DIR *dr;
		struct dirent *d;
		dr = opendir(".");

		if (dr == NULL) {
			printf("Direktori tidak dapat ditemukan" );
		}
		else {
			i = 0;
			char workd[1024], subdir[1024];
			getcwd(workd, sizeof(workd));
			while((d = readdir(dr)) != NULL){
				if(cekfile(d->d_name)) {
					strcpy(subdir, workd);
					strcat(subdir, "/");
					strcat(subdir, d->d_name);
					pthread_create(&(tid[i]), NULL, pindah, (void *)subdir);
					pthread_join(tid[i], NULL);
					i++;
				}
			}
			closedir(dr);
		}
	}

	else if(strcmp(argv[1], "-d") == 0 && argc == 3) {
		chdir(argv[2]);
		DIR *dr;
		struct dirent *d;
		dr = opendir(".");
		if (dr == NULL) {
			printf("Direktori tidak dapat ditemukan" );
		}
		else {
		i = 0;
			char workd[1024], subdir[1024];
			getcwd(workd, sizeof(workd));
			while((d = readdir(dr)) != NULL){
				if(cekfile(d->d_name)) {
					strcpy(subdir, workd);
					strcat(subdir, "/");
					strcat(subdir, d->d_name);
					pthread_create(&(tid[i]), NULL, pindahd, (void *)subdir);
					pthread_join(tid[i], NULL);
					i++;
				}
			}
			closedir(dr);
		}
	}
	//jika argumen yang diinputkan tidak sesuai
	else {
		printf("argumen yang diinput salah\n");
	}

	return 0;
}
