#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/ipc.h> 
#include <sys/shm.h> 
#include <unistd.h>

#define N 4
#define M 2
#define K 5
#define MAX_THREADS N * K
#define ll unsigned long long

struct number{
	ll value;
};

void *penjumlahan(void* arg) {
	struct number* angka;
	angka = (struct number *) arg;

	ll val = angka->value;
	val = (((val+1)*val)/2);
	
	printf("%llu\t", val);
}

int main(int argc, char* argv[]){
	int counter = 0;
	key_t key = 1500;
	int *test, pindah[20];

	int shmid = shmget(key, 20, IPC_CREAT | 0666);

	test = (int *)shmat(shmid, 0, 0);
	
	printf("Matriks dari Shared Memory soal a :\n");
	for(int l=0; l<MAX_THREADS; l++) {
		pindah[l] = test[l];
		printf("%d\t", test[l]);
		if((l%5) == 4) {
			printf("\n");
		}
	}
	shmdt((void *) test);
	shmctl(shmid, IPC_RMID, NULL);
	
	printf("\nMatriks Penjumlahan :\n");
	
	pthread_t threads[MAX_THREADS];

	for(int i=0; i<MAX_THREADS; i++) {
		struct number* angka = (struct number*)malloc(sizeof(struct number));
		angka->value = pindah[i];

		pthread_create(&threads[i], NULL, penjumlahan, (void *)angka);
		pthread_join(threads[i], NULL);
		
		if((i%5) == 4) {
			printf("\n");
		}	
	}
	for(int i=0; i<MAX_THREADS; i++) {
		pthread_join(threads[i], NULL);
	}
	return 0;
}


		
