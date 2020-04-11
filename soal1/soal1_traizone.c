#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdbool.h>
#include <termios.h>

int (*item)[4], (*current_pokemon)[2], *isRunning;
int aktif = 0, pokedollar = 100, pokeball = 10, lul_powder = 0, berry = 0;

bool nrm_cari = false, nrm_pokedex = false, nrm_shop = false,
     capture_mode = false, AP_decreasing = true;

pthread_t tid1[20], 	// untuk menghandle pengurangan AP, efek lullaby, dan input.
	  tid2, 	// untuk melakukan pencarian pokemon setiap 10 detik.
	  tid3;		// untuk menghandle kemungkinan pokemon kabur yang didapat 
			// dari fitur cari pokemon.

struct Pokemon {
	char name[30];
	bool exist;
	double encounter_rate;
	double escape_rate;
	double capture_rate;
	int value_dollar;
	int AP;
} pokemon[7], temp_pokemon;

struct Pokemon *(pokemonAP[7]);

void normal();
void capture();
void pokedex();
void shop();
void pokemon_type();
void *cari_pokemon();
void *AP_decrease();
void *pokemon_escape();
void *lullaby_powder();

int random_number(int number) {
	return rand() % number;
}

bool chance(double chances) {
	return rand() < chances * ((double)RAND_MAX + 1.0);
}

/* NORMAL MODE - 1. CARI POKEMON
   - tiap 10 detik punya 60% chance dapat pokemon sesuai encounter rate.
   - hanya nentuin dapat atau tidak.
   - menu CARI POKEMON berubah jadi BERHENTI MENCARI saat cari pokemon aktif.
   - state mencari pokemon berhenti kalo nemu pokemon atau diberentiin player.
   - kalo nemu pokemon masuk ke CAPTURE MODE.
   - (opsional) nambah menu buat redirect ke CAPTURE MODE dari NORMAL MODE.
*/

void *cari_pokemon(void *ptr) {
	tid2 = pthread_self();
	while(aktif) {
		sleep(10);
		if(chance(0.6)) {
			printf("\n\nYou Catch a Pokemon!\nSelect : ");
			fflush(stdout);
			nrm_cari = true;
			aktif = 0;
		} else {
			printf("\n\nNo Pokemon Found, Keep Looking.\nSelect : ");
			fflush(stdout);
		}
	}
}

void pokemon_type() {
	char type_normal[5][10] = 
	     {"Bulbasaur", "Charmander", "Squirtle", "Rattata", "Caterpie"};
	char type_rare[5][10] = 
	     {"Pikachu", "Eevee", "Jigglypuff", "Snorlax", "Dragonite"};
	char type_legend[5][10] = 
	     {"Mew", "Mewtwo", "Moltres", "Zapdos", "Articuno"};

	int random1 = random_number(5);
	if(*current_pokemon[0] == 3) {
		strcpy(temp_pokemon.name, type_legend[random1]);
		temp_pokemon.encounter_rate = 0.05;
		temp_pokemon.escape_rate = 0.2;
		temp_pokemon.capture_rate = 0.3;
		temp_pokemon.value_dollar = 200;
	} else if(*current_pokemon[0] == 2) {
		strcpy(temp_pokemon.name, type_rare[random1]);
		temp_pokemon.encounter_rate = 0.15;
		temp_pokemon.escape_rate = 0.1;
		temp_pokemon.capture_rate = 0.5;
		temp_pokemon.value_dollar = 100;
	} else {
		strcpy(temp_pokemon.name, type_normal[random1]);
		temp_pokemon.encounter_rate = 0.8;
		temp_pokemon.escape_rate = 0.05;
		temp_pokemon.capture_rate = 0.7;
		temp_pokemon.value_dollar = 80;
	}
	temp_pokemon.exist = false;
	temp_pokemon.AP = 100;

	if(*current_pokemon[1] == 1) {
		strcat(temp_pokemon.name, " - SHINING");
		temp_pokemon.escape_rate += 0.05;
		temp_pokemon.capture_rate -= 0.2;
		temp_pokemon.value_dollar += 5000;
	}
}

void *AP_decrease(void *pokemonAP) {
	while(((struct Pokemon*)pokemonAP)->exist == false) {
		while(((struct Pokemon*)pokemonAP)->AP <= 100 && capture_mode == false) {
			sleep(10);
			((struct Pokemon*)pokemonAP)->AP -= 10;

			if(((struct Pokemon*)pokemonAP)->AP == 0) {
				if(chance(0.9)) {
					printf("Pokemon %s Kabur!\n", ((struct Pokemon*)pokemonAP)->name);
					((struct Pokemon*)pokemonAP)->exist = true;
					break;
				} else {
					printf("AP Pokemon %s Reset Menjadi 50\n", ((struct Pokemon*)pokemonAP)->name);
					((struct Pokemon*)pokemon)->AP = 50;
				}
			}
		}
	}
}

void *pokemon_escape(void *ptr) {
	tid3 = pthread_self();
	
	while(capture_mode) {
		sleep(20);
		if(chance(temp_pokemon.escape_rate)) {
			printf("Pokemon Kabur!\n");
			printf("kembali ke MENU . . .\n");
			capture_mode = false;
			sleep(1);
			break;
		}
	}
}

void *lullaby_powder(void *ptr) {
	temp_pokemon.escape_rate = 0.0;
	double temp_escape_rate = temp_pokemon.escape_rate;
	temp_pokemon.capture_rate += 0.2;
	sleep(10);
	temp_pokemon.escape_rate = temp_escape_rate;
	temp_pokemon.capture_rate -= 0.2;
}

/* NORMAL MODE - 2. POKEDEX
   - tampilkan list pokemon dan APnya.
   - maksimal pokemon 7.
   - kalau menangkap >7, yang baru ditangkap langsung dilepas tapi dpt pokedollar.
   - initial value AP = 100 berkurang -10 AP per 10 detik mulai waktu ditangkap.
   - jika AP = 0, pokemon punya 90% chance kabur tanpa ngasi pokeball dan
     10% chance untuk reset AP jadi 50.
   - AP tidak berkuran di CAPTURE MODE.
   - bisa ngelepas pokemen yang ditangkap dan dapat pokedollar.
   - bisa memberi berry ke semua pokemon untuk naikin AP +10 (1 berry untuk semua).
*/

void pokedex() {
	char input_pokemon[20];

	while(nrm_pokedex) {
		int flag = 0;
		printf("\n--== POKEDEX ==--");
		printf("\n(1) Lihat Pokemon");
		printf("\n(2) Beri Makan Berry");
		printf("\n(3) Lepas Pokemon");
		printf("\n(4) Exit");
		printf("\n Select : ");

		fgets(input_pokemon, 20, stdin);
		input_pokemon[strlen(input_pokemon)-1] = '\0';

		if(strcmp("Lihat Pokemon", input_pokemon)==0 || strcmp("1", input_pokemon) == 0) {
			for(int i=0; i<7; i++) {
				if(pokemon[i].exist == false) {
					printf("POKEMON KE-%d :\n", i+1);
					printf("Nama : %s\n", pokemon[i].name);
					printf("AP   : %d\n", pokemon[i].AP);
					flag = 1;
				}
				if(!flag) {
					printf("Anda Belum Punya Pokemon.\n");
				}
			}
		} else if(strcmp("Beri Makan Berry", input_pokemon)==0 || strcmp("2", input_pokemon) == 0) {
			if(berry > 0) {
				for(int i=0; i<7; i++) {
					if(pokemon[i].exist) {
						pokemon[i].AP += 10;
						flag = 1;
					}
				}
				if(flag) {
					berry--;
					printf("Berry Telah Diberikan.\n");
				} else {
					printf("Anda Belum Punya Pokemon.\n");
				}
			} else {
				printf("Anda Tidak Punya Berry.\n");
			}
		} else if(strcmp("Lepas Pokemon", input_pokemon)==0 || strcmp("3", input_pokemon) == 0) {
			for(int i=0; i<7; i++) {
				if(!pokemon[i].exist) {
					printf("Inventory %d : %s\n", i+1, pokemon[i].name);
					flag = 1;
				}
			}
			if(flag) {
				printf("Pilih Pokemon yang Mau Dilepas : ");
				int lepas;				
				scanf("%d", &lepas);
				getchar();
				if(!pokemon[lepas-1].exist && lepas > 0) {
					pokemon[lepas-1].exist = true;
					pokedollar += pokemon[lepas-1].value_dollar;
					printf("Pokemon %s Berhasil Dilepas.\n", pokemon[lepas-1].name);
					printf("Anda Mendapat %d Pokedollar.\n", pokemon[lepas-1].value_dollar);
				} else {
					printf("Pilihan Tidak Valid\n");
				}
			} else {
				printf("Anda Tidak Punya Pokemon\n");	
			}
		} else if(strcmp("Exit", input_pokemon)==0 || strcmp("4", input_pokemon) == 0) {
			printf("kembali ke MENU . . .\n");
			nrm_pokedex = false;
			sleep(1);
			break;
		} else {
			printf("Pilihan Tidak Valid\n");
		}
	}
}

/* NORMAL MODE - SHOP
   - membeli item dari soal2_pokezone.
   - max item yang dapat dibeli dan dimiliki player adalah 99.
*/  

void shop() {
	int inventory = 99 - (pokeball + lul_powder + berry), jml;
	int hrg_pokeball = 5, hrg_lulpowder = 60, hrg_berry = 15;	
	char input_shop[20];

	while(nrm_shop) {
		printf("\n--== SHOP ==--");
		printf("\n(1) Beli Pokeball {%d}", *item[1]);
		printf("\n(2) Beli Lullaby Powder {%d}", *item[2]);
		printf("\n(3) Beli Berry {%d}", *item[3]);
		printf("\n(4) Exit");
		printf("\n\n--== INVENTORY ==--");
		printf("\nAvailable Inventory Space : %d", inventory);
		printf("\n[Pokeball - %d]  [Lullaby Powder - %d]  [Berry - %d]", pokeball, lul_powder, berry);
		printf("\nPokedollar : %d", pokedollar);
		printf("\n Select : ");

		fgets(input_shop, 20, stdin);
		input_shop[strlen(input_shop)-1] = '\0';

		if(strcmp("1", input_shop) == 0) {
			printf("Beli Pokeball. Jumlah : \n");
			scanf("%d", &jml);

			if((inventory-jml) >= 0 || jml <= 99) {
				if(pokedollar >= jml*hrg_pokeball) {
					pokedollar -= jml*hrg_pokeball;
					pokeball += jml;
					inventory -= jml;
					*item[1] -= jml;
					printf("%d Pokeball Berhasil Dibeli.\n", jml);
				} else {
					printf("Pokedollar Tidak Cukup.\n");
				}
			} else {
				printf("Inventory Tidak Cukup.\n");
			}
		} else if(strcmp("2", input_shop) == 0) {
			printf("Beli Lullaby Powder. Jumlah : \n");
			scanf("%d", &jml);

			if((inventory-jml) >= 0 || jml <= 99) {
				if(pokedollar >= jml*hrg_lulpowder) {
					pokedollar -= jml*hrg_lulpowder;
					lul_powder += jml;
					inventory -= jml;
					*item[2] -= jml;
					printf("%d Lullaby Powder Berhasil Dibeli.\n", jml);
				} else {
					printf("Pokedollar Tidak Cukup.\n");
				}
			} else {
				printf("Inventory Tidak Cukup.\n");
			}
		} else if(strcmp("3", input_shop) == 0) {
			printf("Beli Berry. Jumlah : \n");
			scanf("%d", &jml);

			if((inventory-jml) >= 0 || jml <= 99) {
				if(pokedollar >= jml*hrg_berry) {
					pokedollar -= jml*hrg_berry;
					berry += jml;
					inventory -= jml;
					*item[3] -= jml;
					printf("%d Berry Berhasil Dibeli.\n", jml);
				} else {
					printf("Pokedollar Tidak Cukup.\n");
				}
			} else {
				printf("Inventory Tidak Cukup.\n");
			}
		} else if(strcmp("4", input_shop) == 0) {
			printf("kembali ke MENU . . .\n");
			nrm_shop = false;
			sleep(1);
			break;
		} else {
			printf("Pilihan Tidak Valid\n");
		}
		getchar();
	}
}

/* CAPTURE MODE
   - TANGKAP -> menangkap dengan pokeball, stok di onventory -1 setiap digunakan.
   - ITEM -> menjalankan efek lullaby powder.
   - KELUAR -> keluar dari CAPTURE MODE ke NORMAL MODE.
   - pokemon memiliki peluang kabur sesuai persentasi escape_rate.
*/

void capture() {
	char input_capture[20];
	int i;
	nrm_cari = false;
	pokemon_type();

	if(pthread_create(&tid1[2], NULL, pokemon_escape, NULL)) {
		printf("thread 3 failed\n");
	}
	
	while(capture_mode) {
		printf("\n--== CATCH THE POKEMON! ==--");
		printf("\n(1) Tangkap Pokemon");
		printf("\n(2) Item");
		printf("\n(3) Exit");
		printf("\n Select : ");

		fgets(input_capture, 20, stdin);
		input_capture[strlen(input_capture)-1] = '\0';
	
		if(strcmp("1", input_capture) == 0) {
			if(pokeball != 0) {
				pokeball--;
				sleep(1);
				if(chance(temp_pokemon.capture_rate)) {
					printf("Pokemon %s Captured!\n", temp_pokemon.name);
					sleep(1);
					for(i=0; i<7; i++) {
						if(pokemon[i].exist) {
							pokemon[i] = temp_pokemon;
							printf("%s Disimpan di Pokedex.\n", temp_pokemon.name); 
							if(pthread_create(&tid1[i+4], NULL, AP_decrease, pokemonAP[i])) {
								printf("thread %d failed\n", i+4);
							}
							break;
						}
					}
					if(i == 7) {
						printf("Slot Pokedex Full.\n");
						printf("Pokemon Dilepas dan Mendapat %d Pokedollar.\n", temp_pokemon.value_dollar);
					}
				} else {
					printf("No Pokemon is Captured.\n");
				}
			} else {
				printf("Pokeball Tidak Cukup.\n");
			}
			pthread_cancel(tid3);

			printf("kembali ke MENU . . .\n");
			capture_mode = false;
			sleep(1);
			break;
		} else if(strcmp("2", input_capture) == 0) {
			if(lul_powder != 0) {
				if(pthread_create(&tid1[3], NULL, lullaby_powder, NULL)) {
					printf("thread 4 failed\n");
				}
				printf("Lullaby Powder Dipakai Oleh %s.\n", temp_pokemon.name);
			} else {
				printf("Lullaby Powder Tidak Mencukupi.\n");
			}
		} else if(strcmp("3", input_capture) == 0) {
			pthread_cancel(tid3);
			printf("kembali ke MENU . . .\n");
			capture_mode = false;
			sleep(1);
			break;
		} else {
			printf("Pilihan Tidak Valid\n");
		}
	}
	return;
}

/* NORMAL MODE */

void normal() {
	char choice1[20], choice2[20] = "cari pokemon", choice3[20] = "berhenti mencari";
	char input_normal[20];

	strcpy(choice1, choice2);

	printf("--== SELAMAT DATANG DI POKE*ZONE!! ==--\n");
	while(!(capture_mode && nrm_pokedex && nrm_shop)) {
		if(!aktif) {
			strcpy(choice1, choice2);
		} else {
			strcpy(choice1, choice3);
		}
		printf("\n--== MAIN MENU ==--");
		printf("\n(1) %s", choice1);
		printf("\n(2) Pokedex");
		printf("\n(3) Shop");
		printf("\n(4) Capture Mode");
		printf("\n(5) Exit");
		printf("\n Select : ");
		
		fgets(input_normal, 20, stdin);
		input_normal[strlen(input_normal)-1] = '\0';

		if(strcmp("1", input_normal) == 0) {
			if(pthread_create(&tid1[0], NULL, cari_pokemon, NULL)) {
				printf("thread 1 failed\n");
			}
			if(aktif) {
				pthread_cancel(tid2);
				aktif = 0;
			} else {
				aktif = 1;
			}
		} else if(strcmp("2", input_normal) == 0) {
			nrm_pokedex = true;
			pokedex();
		} else if(strcmp("3", input_normal) == 0) {
			nrm_shop = true;
			shop();
		} else if(strcmp("4", input_normal) == 0) {
			if(nrm_cari) {
				aktif = 0;
				printf("\nMemindahkan ke CAPTURE MODE . . .\n");
				sleep(1);
				capture_mode = true;
				capture();
			} else {
				printf("Belum Menemukan Pokemon\n");
			}
		} else if(strcmp("5", input_normal) == 0) {
			break;
		} else {
			printf("Pilihan Tidak Valid\n");
		}
		printf("\n");
	}
}

int main() {
	key_t key = 1600;
	int shmid = shmget(key, sizeof(int), IPC_CREAT | 0666);
	item = shmat(shmid, NULL, 0);
	current_pokemon = shmat(shmid, NULL, 0);
	isRunning = shmat(shmid, NULL, 0);

	for(int i=0; i<7; i++) {
		pokemon[i].exist = true;
		pokemonAP[i] = (struct Pokemon *)malloc(sizeof(struct Pokemon));
		pokemonAP[i] = &pokemon[i];
	}

	normal();
	
	shmdt(item);
  	shmdt(current_pokemon);
  	shmdt(isRunning);
  	shmctl(shmid, IPC_RMID, NULL);

	return 0;
}
