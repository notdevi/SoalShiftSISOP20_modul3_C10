#include<stdio.h>
#include<string.h>
#include<ctype.h>
#include<pthread.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<dirent.h>

pthread_t tid[100];

int cekfile(const char *path) {
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);
}

void* pindah(void *arg) {
    char *path;
    path = (char *) arg;
    char *pisah1, *array1[10], *pisah2, *array2[10];
    int n = 0;
    
    pisah1 = strtok(path, ".");
    while(pisah1 != NULL) {
        // printf( "token %d = %s\n", n , pisah1);
        array1[n] = pisah1;
        n++;
        pisah1 = strtok(NULL, ".");
    }

    printf("%s\n", array1[n-1]);       // nampilin ekstensi

    int m = 0;
    pisah2 = strtok(path, "/");
    while(pisah2 != NULL) {
        array2[m] = pisah2;
        // printf( "token %d = %s\n", m , array2[m]);
        m++;
        pisah2 = strtok(NULL, "/");
    }
    
    sprintf(array2[m-1], "%s.%s", array2[m-1], array1[n-1]);
    // strcat(array2[m-1], arr);
    // strcat(array2[m-1], array1[n-1]);
    printf("%s\n", array2[m-1]);       // nampilin nama file

    char ekstensi[50];
    strcpy(ekstensi, array1[n-1]);
    for(int x=0; ekstensi[x]; x++){
        ekstensi[x] = tolower(ekstensi[x]);
    }    

    /* buat folder dari pisahan, ngecek folder yg namanya berupa ext uda ada apa belum, kalo blm dibuat,
    setelah dibuat dipindah sesuai ext */

    struct dirent *drct;
    DIR *dir = opendir(workingdir), *diropen;
    struct stat sb;

    if(!(stat(dirname, &sb)==0 && S_ISDIR(sb.st_mode))) {
        mkdir(ekstensi, 0777);
    }
}

int main(int argc, char const *argv[]){
    int x;
    if(strcmp(argv[1], "-f") == 0) {
        // while (argv[i] != NULL) {
        //     printf("%s", argv[i]);
        //     i++;
        // }
        for(x=0; x < argc-2; x++){
            pthread_create(&(tid[x]), NULL, pindah, (void *)argv[x+2]);
        }
        for(x=0; x<argc-2; x++){
            pthread_join(tid[x], NULL);
        }
    }
    // else if(strcmp(argv[1], "*") == 0) {
        
    // }
    // else if(strcmp(argv[1], "-d") == 0) {
        
    // }
    else {
        printf("Argument Tidak Benar\n");
    }
}
