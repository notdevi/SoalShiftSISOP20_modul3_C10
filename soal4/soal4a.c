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

int matA[N][M] = {{1, 2}, {2, 3}, {3, 4}, {4, 5}};
int matB[M][K] = {{1, 3, 5, 7, 9}, {2, 4, 6, 8, 10}};
int matC[N][K];

struct val {
	int i, j;
};

void *multp(void *arg) {
	struct val *data = arg;
	int sum = 0;

	for(int i=0; i<M; i++) {
		sum += matA[data->i][i] * matB[i][data->j];
	}

	matC[data->i][data->j] = sum;
	pthread_exit(0);
}

int main(int argc, char **argv) {
	int counter = 0;
	key_t key = 1500;
	int *test, l = 0;

	int sizemem = sizeof(*test);
	int shmid = shmget(key, 20, IPC_CREAT | 0666);

	test = (int *)shmat(shmid, 0, 0);
	
	pthread_t threads[MAX_THREADS];
	for(int i=0; i<N; i++) {
		for(int j=0; j<K; j++) {
			struct val *data = (struct val *)malloc(sizeof(struct val));
			data->i = i;
			data->j = j;	
			pthread_create(&threads[counter], NULL, multp, data);
			pthread_join(threads[counter], NULL);
			counter++;
		}
	}
	for(int i=0; i<MAX_THREADS; i++) {
		pthread_join(threads[i], NULL);
	}
	for(int i=0; i<N; i++) {
		for(int j=0; j<K; j++) {
			test[l] = matC[i][j];
			l++;
			printf("%d\t", matC[i][j]);

		}
		printf("\n");
	}
	shmdt((void *) test);
	// shmctl(shmid, IPC_RMID, NULL);
	return 0;
}

	


