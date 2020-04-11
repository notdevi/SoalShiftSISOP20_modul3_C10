#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdbool.h>
#include <wait.h>
#include <termios.h>

int (*item)[4], (*current_pokemon)[2], *isRunning;

/* SHUTDOWN GAME 
   - mematikan program soal1_pokezone dan soal1_traizone.
*/

void shutdown() {
	pid_t child_id;
	int status;
	
	child_id = fork();
	if(child_id == 0) {
		char *argv[] = {"killall", "soal1_traizone", NULL};
		execv("/usr/bin/killall", argv);
	} else {
		while((wait(&status)) > 0);
//		char *argv[] = {"killall", "soal1_pokezone", NULL};
//		execv("/usr/bin/killall", argv);
	}
}

/* JUAL ITEM
   - stok awal semua item adalah 100.
   - masing-masing item akan bertambah +10 item/10 detik.
   - maximum item dalam shop adalah 200.
   - implementasi lullaby powder dibuat masing-masing 1 thread.
*/

void *jual_item(void *ptr) {
	sleep(10);
	while(*item[1] <= 200 || *item[2] <= 200 || *item[3] <= 200) {
		for(int i=0; i<4; i++) {
			if(*item[i] <= 200) {
				*item[i] += 10;
				if(*item[i] > 200){
					*item[i] = 200;
				}
			}
		}
		sleep(10);
	}
}

/* RANDOM POKEMON
   - setiap tipe pokemon punya peluang 1/8000 untuk jadi shiny pokemon.
   - shiny pokemon meningkatkan escape rate +5%, capture rate -20%, pokedollar
     dari melepas +5000.
   - setiap 1 detik thread akan mengkalkulasi random ulang untuk diberikan
     ke soal2_traizone.
*/

double chance(double chances) {
	return chances * ((double)RAND_MAX + 1.0);
}

void *random_pokemon(void *ptr) {
	while(1) {
		double random = rand();
		double normal = chance(0.8);
		double rare = normal + chance(0.15);

		if(random > normal) {
			if(random > rare) {
				*current_pokemon[0] = 3;
			} else {
				*current_pokemon[0] = 2;
			}
		} else {
			*current_pokemon[0] = 1;
		}
		if((rand() < chance(0.000125))) {
			*current_pokemon[1] = 1;
		} else {
			*current_pokemon[1] = 0;
		}
		sleep(1);
	}
}

int main() {
	int running = 1;

	key_t key = 1600;
	int shmid = shmget(key, sizeof(int), IPC_CREAT | 0666);
  	item = shmat(shmid, NULL, 0);
  	current_pokemon = shmat(shmid, NULL, 0);
  	isRunning = shmat(shmid, NULL, 0);

	isRunning = &running;

	for(int i=0; i<4; i++) {
		*item[i] = 100;
	}
	
	pthread_t tid[3];

	if(pthread_create(&tid[0], NULL, jual_item, NULL)){
    		printf("thread 1 failed\n");
  	}
	if(pthread_create(&tid[1], NULL, random_pokemon, NULL)){
    		printf("thread 2 failed\n");
	}
	
	while(running) {
		int input;

		printf("masukkan 1 untuk shutdown program\n");
		printf(">> ");
		scanf("%d", &input);
		
		if(input == 1) {
			shutdown();
			running = 0;
		}
	}
	
	shmdt(item);
  	shmdt(current_pokemon);
  	shmdt(isRunning);
  	shmctl(shmid, IPC_RMID, NULL);

	return 0;
} 
