# SoalShiftSISOP20_modul3_C10
Praktikum Modul 3 Sistem Operasi 2020

### Nama Anggota Kelompok :
### 1. Devi Hainun Pasya (05111840000014)
### 2. Anggara Yuda Pratama (05111840000008)

### Soal No. 1

Buatlah game Pokemon GO. Ketentuan permainan sebagai berikut :

(a) Menggunakan IPC-shared memory, thread, fork-exec.

(b) Bebas berkreasi asal tidak konflik dengan requirements yang ada.

(c) Terdapat 2 code soal yaitu `soal2_traizone.c` dan `soal2_pokezone.c`.

(d) soal2_traizone.c mengandung fitur :
	
NORMAL MODE - 1. CARI POKEMON

   - tiap 10 detik punya 60% chance dapat pokemon sesuai encounter rate.
   - hanya nentuin dapat atau tidak.
   - menu CARI POKEMON berubah jadi BERHENTI MENCARI saat cari pokemon aktif.
   - state mencari pokemon berhenti kalo nemu pokemon atau diberentiin player.
   - kalo nemu pokemon masuk ke CAPTURE MODE.
   - (opsional) nambah menu buat redirect ke CAPTURE MODE dari NORMAL MODE.

Yang pertama dilakukan adalah mendeklarasikan state dari masing-masing mode dengan tipe data `boolean`, apabila mode sedang aktif, maka `true`, sebaliknya apabila mode sedang tidak aktif maka `false`.
```c
bool nrm_cari = false, nrm_pokedex = false, nrm_shop = false,
     capture_mode = false, AP_decreasing = true;
```
Kemudian mendeklarasi item beserta mata uang dalam game. Item direpresentasikan sebagai `array` satu dimensi. Array `current_pokemon[2]` memiliki 2 indeks, yang pertama yaitu untuk mengidentifikasi tipe pokemon, sedangkan indeks kedua mengidentifikasi shining/tidaknya pokemon. 
```c
int (*item)[4], (*current_pokemon)[2], *isRunning;
int aktif = 0, pokedollar = 100, pokeball = 10, lul_powder = 0, berry = 0;
```
Tiap-tiap pokemon memiliki karakteristik yang berbeda, sehingga dideklarasikan menggunakan `struct` bernama `Pokemon`.
```c
struct Pokemon {
	char name[30];
	bool exist;
	double encounter_rate;
	double escape_rate;
	double capture_rate;
	int value_dollar;
	int AP;
} pokemon[7], temp_pokemon;
```
Dalam soal ini digunakan 3 buah thread yaitu :
 - `tid1[20]` yang berfungsi untuk menghandle pengurangan AP (Affection Point), efek Lullaby Powder, dan input.
 - `tid2` yang berfungsi untuk melakukan pencarian pokemon setiap 10 detik sekali.
 - `tid3` yang berfungsi untuk menghandle kemungkinan pokemon kabur yang didapat dari fitur pencarian pokemon.
 
Untuk mengacak tipe-tipe pokemon, dibuat sebuah fungsi yang men-generate random number. Dan untuk mengacak kemungkinan dapatnya pokemon, digunakan fungsi yang men-generate angka bertipe data `double`, karena pada soal digunakan persen.
```c
int random_number(int number) {
	return rand() % number;
}

bool chance(double chances) {
	return rand() < chances * ((double)RAND_MAX + 1.0);
}
```
Pada fungsi `void *cari_pokemon();`, dilakukan proses pencarian pokemon yang akan berjalan selama mode `cari pokemon` aktif. Apabila kemungkinan yang didapat dari fungsi `chance` memenuhi, maka pokemon berhasil didapatkan, dan state `nrm_cari` menjadi `true`. Sebaliknya, apabila pokemon tidak berhasil didapatkan akan keluar notifikasi "tidak ditemukan pokemon".
```c
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
```
Pada fungsi `void pokemon_type();`, dilakukan pendeklarasian nama-nama, karakteristik pokemon beserta tipenya. `temp_pokemon.exist` di set `false` dan AP default di set sebanyak 100. 
```c
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
```
Apabila indeks kedua dari array `current_pokemon` bernilai 1, maka pokemon tersebut SHINING. Sehingga, `escape_rate` bertambah, `capture_rate` berkurang, dan `value_dollar` bertambah.
```c
if(*current_pokemon[1] == 1) {
		strcat(temp_pokemon.name, " - SHINING");
		temp_pokemon.escape_rate += 0.05;
		temp_pokemon.capture_rate -= 0.2;
		temp_pokemon.value_dollar += 5000;
	}
}
```
Pada fungsi `void *AP_decrease();`, akan dilakukan operasi pengurangan AP per 10 detik. Apabila AP mencapai 0, maka pokemon bisa lepas atau APnya di reset menjadi 50.
```c
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
```
Pada fungsi `void *pokemon_escape();`, pada saat permainan sedang dalam CAPTURE MODE, pokemon yang ditemukan memiliki kemungkinan untuk kabur. Setiap 20 detik, apabila `escape_rate` yang didapat dari fungsi `chance` memenuhi, maka pokemon akan kabur.
```c
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
```
Pada fungsi `void *lullaby_powder();`, dijalankan efek dari item Lullaby Powder. `escape_rate` dari pokemon akan di set ke 0, dan `capture_rate` bertambah 20% atau 0.2. Saat efeknya hilang akan dikembalikan ke kondisi awal.
```c
void *lullaby_powder(void *ptr) {
	temp_pokemon.escape_rate = 0.0;
	double temp_escape_rate = temp_pokemon.escape_rate;
	temp_pokemon.capture_rate += 0.2;
	sleep(10);
	temp_pokemon.escape_rate = temp_escape_rate;
	temp_pokemon.capture_rate -= 0.2;
}
```

NORMAL MODE - 2. POKEDEX
   - tampilkan list pokemon dan APnya.
   - maksimal pokemon 7.
   - kalau menangkap >7, yang baru ditangkap langsung dilepas tapi dpt pokedollar.
   - initial value AP = 100 berkurang -10 AP per 10 detik mulai waktu ditangkap.
   - jika AP = 0, pokemon punya 90% chance kabur tanpa ngasi pokeball dan
     10% chance untuk reset AP jadi 50.
   - AP tidak berkuran di CAPTURE MODE.
   - bisa ngelepas pokemen yang ditangkap dan dapat pokedollar.
   - bisa memberi berry ke semua pokemon untuk naikin AP +10 (1 berry untuk semua).

Pada fungsi `void pokedex()`, selama dalam mode pokedex `nrm_pokedex = true`, pertama-tama akan ditampilkan option dari menu pokedex. Kemudian player akan diminta menginput option yang diinginkan dengan fungsi `fgets(input_pokemon, 20, stdin);`.
```c
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
```
Apabila player menginput "1" maka akan ditampilkan list pokemon yang berhasil ditangkap. Ada tidaknya pokemon dicek dengan kondisi `if(pokemon[i].exist == false)`. Apabila pokemon pada indeks tertentu tidak ada, maka keluar notifikasi `printf("Anda Belum Punya Pokemon.\n");`.
```c
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
		}
```
Apabila player menginput "2" maka player akan memberikan berry ke pokemon. Pertama akan dicek apakah terdapat stok berry `if(berry > 0)`, jika ada maka berry akan diberikan ke semua pokemon dan APnya akan bertambah 10, dan stok berry berkurang. Jika berry tidak cukup akan keluar notifikasi. 
```c
		else if(strcmp("Beri Makan Berry", input_pokemon)==0 || strcmp("2", input_pokemon) == 0) {
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
		}
```
Apabila player menginput "3", maka player akan melepas suatu pokemon. Pertama akan dilakukan pengecekkan ada/tidaknya pokemon pada pokedex. Lalu ditampilkan list pokemon.
```c
			for(int i=0; i<7; i++) {
				if(!pokemon[i].exist) {
					printf("Inventory %d : %s\n", i+1, pokemon[i].name);
					flag = 1;
				}
			}
```
Apabila `flag == 1`, maka pokemon ada dan player akan diminta menginput pokemon yang hendak dilepas.
```c
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
		}
```
Apabila player menginput "4", maka akan keluar dari menu pokedex dengan men-set `nrm_pokedex = false;`. Selain input diatas, tidak dianggap valid.
```c
else if(strcmp("Exit", input_pokemon)==0 || strcmp("4", input_pokemon) == 0) {
			printf("kembali ke MENU . . .\n");
			nrm_pokedex = false;
			sleep(1);
			break;
		} else {
			printf("Pilihan Tidak Valid\n");
		}
```



### Soal No. 2

Qiqi adalah sahabat MamMam dan Kaka. Qiqi , Kaka dan MamMam sangat senang bermain “Rainbow six” bersama-sama , akan tetapi MamMam sangat Toxic ia selalu melakukan Team killing kepada Qiqi di setiap permainannya. Karena Qiqi orang yang baik hati, meskipun marah Qiqi selalu berkata “Aku nggk marah!!”. Kaka ingin meredam kemarahan Qiqi dengan membuatkannya sebuah game yaitu TapTap Game. akan tetapi Kaka tidak bisa membuatnya sendiri, ia butuh bantuan mu. Ayo!! Bantu Kaka menenangkan Qiqi.

TapTap Game adalah game online berbasis text console. Terdapat 2 program yaitu tapserver.c dan tapplayer.c

Pada `tapplayer.c` dapat melakukan:
- login dan register
- Jika login berhasil maka akan menampilkan pesan “login success”, jika gagal akan menampilkan pesan “login failed” (pengecekan login hanya mengecek username dan password, maka dapat multi autentikasi dengan username dan password yang sama)
- Pada screen 1 kalian juga dapat menginputkan “register”, setelah menekan enter anda diminta untuk menginputkan username dan password sama halnya seperti login
- Pada register tidak ada pengecekan unique username, maka setelah register akan langsung menampilkan pesan “register success” dan dapat terjadi double account
- Setelah itu kita dapat melakukan find match

Sedangkan pada `tapserver.c` dapat melakukan :
- Pada saat program pertama kali dijalankan maka program akan membuat file akun.txt jika file tersebut tidak ada. File tersebut digunakan untuk menyimpan username dan password
- Pada saat user berhasil login maka akan menampilkan pesan “Auth success” jika gagal “Auth Failed”
- Pada saat user sukses meregister maka akan menampilkan List account yang terdaftar (username dan password harus terlihat)

Sebelum itu mohon maaf mas kodingan kami hanya mampu login dan register dan itu juga masih belum sempurna.

***tapplayer.c***

[kodingan](https://github.com/notdevi/SoalShiftSISOP20_modul3_C10/blob/master/soal2/tapplayer.c)

***tapserver.c***

[kodingan](https://github.com/notdevi/SoalShiftSISOP20_modul3_C10/blob/master/soal2/tapserver.c)

Berikut pada kodingan `tapplayer.c` yang dapat melakukan login serta register.
```c
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#define PORT 4444
#define LEN 40      //panjang string user dan password
#define MAX 100     //maksimal 100 orang yang bisa regis

int main(int argc, char const *argv[]) {
    struct sockaddr_in address;
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char msg[1024];
    //pesan yang akan dipassing tapserver
    char *msg1 = "Auth success";
    char *msg2 = "Auth failed";
    char user[1024], sandi[1024];  //nampung username sama password
    char listname[100][40];         //array 2 dimensi, karena butuh beberapa orang yang bisa register
    char listpass[100][40];
    
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }
  
    memset(&serv_addr, '0', sizeof(serv_addr));
  
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
      
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }
  
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }
    // printf("cb\n");
    int count = 0;      //ngeflag buat berapa orang yang regis, kalo uda regis countnya nambah
    while(1){
        char username[LEN];
        char pass[LEN];
        char buffer[1024] = {0};
        int i, a;

        scanf("%s", msg);                    //scan inputan login atau register
        send(sock , msg , sizeof(msg) , 0 ); //ngirim ke server
 
        // printf("scan berhasil\n");   //cek uda kekirim apa belum
	
	//jika yang diinputkan register
        if(strcmp(msg, "register")==0){ 
	        printf("username : ");
	        scanf("%s", listname[count]);
            sprintf(user, "%s\n", listname[count]);    //convert string to integer supaya bisa disend
            send(sock , user , sizeof(user) , 0 );    //ngirim ke server buat dimasukin ke akun.txt
	        printf("password : ");
	        scanf("%s", listpass[count]);
            sprintf(sandi, "%s\n", listpass[count]);
            send(sock , sandi , sizeof(sandi) , 0 );
	        count++;
            printf("register berhasil\n");
        }
	//jika yang register login
        else if(strcmp(msg, "login")==0){
            printf("Username : ");
            scanf("%s", username);
            printf("password : ");
            scanf("%s", pass);
            for(i = 0; i< count; i++){
                if (strcmp(listname[i], username) == 0 && strcmp(listpass[i], pass) == 0) {
                    printf("login success\n");
                    send(sock , msg1 , strlen(msg1) , 0 );  //send msg1 ke server
                }
                else  {
                    printf("login failed\n");
                    send(sock , msg2 , strlen(msg2) , 0 );  //send msg2 ke server
                }
            }
        }
        else printf("Harus register atau login! \n");
    }
    return 0;
}
```

Sedangkan di `tapserver.c` akan menampung data akun yang sudah terbuat dalam file `akun.txt` dan cek apakah autentikasinya berhasil atau gagal.
```c
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#define PORT 4444

int main(int argc, char const *argv[]) {
    int server_fd, new_socket, valread, valread2;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
  
    char *msg = "penambahan berhasil", *data;
      
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
      
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );
      
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    
    FILE* file_ptr;

    // int count = 0;
    while(1){
        file_ptr = fopen("akun.txt", "a");
        char buffer[1024] = {0};
        char buffer2[1024] = {0};
        char buffer3[1024] = {0};

        valread = read( new_socket , buffer, 1024);     //nampung kiriman dari player berupa register atau login
        // printf("%s\n", buffer);
	
	//jika data yang diterima register
        if(strcmp(buffer, "register")==0){
            valread = read( new_socket , buffer2, 1024);    //nampung username
            //pake buffer2 krn dalam satu looping while gak mungkin dipake barengan
            // printf("%s\n", buffer2);

            valread2 = read( new_socket , buffer3, 1024);   //nampung password
            // printf("%s\n", buffer3);

            //masukkin ke akun.txt
            fputs(buffer2, file_ptr);
            fputs(buffer3, file_ptr);
            fclose(file_ptr);
        }
	//jika data yang diterima login
        else if(strcmp(buffer, "login")==0){
            valread = read( new_socket , buffer2, 1024);    //nampung msg1 atau msg2 yang dikirim
            printf("%s\n", buffer2);
        }
    }
    // fclose(file_ptr);
    return 0;
}
```

### Soal No. 3

Buatlah sebuah program C untuk mengkategorikan file. Program memindahkan file sesuai dengan ekstensinya (tidak case sensitive) ke dalam folder sesuai ekstensinya yang folder hasilnya terdapat di working directory ketika program tersebut dijalankan.

Untuk opsi `-f` user bisa menambahkan argumen file yang bisa dikategorikan sebanyak yang diinginkan.
Pada program kategori tersebut, folder jpg,c,zip tidak dibuat secara manual,
melainkan melalui program c. Semisal ada file yang tidak memiliki ekstensi,
maka dia akan disimpan dalam folder “Unknown”.

Untuk opsi `\*` untuk mengkategori seluruh file yang ada di working directory ketika menjalankan program C tersebut. Selain hal itu program C ini juga menerima opsi -d untuk melakukan kategori pada suatu directory. Untuk opsi -d ini, user hanya bisa menginput 1 directory saja, tidak seperti file yang bebas menginput file sebanyak mungkin.

Untuk opsi `-d` digunakan untuk melakukan kategori pasa suatu direktori. Program ini tidak rekursif. Semisal di directory yang mau dikategorikan, atau menggunakan `(*)` terdapat folder yang berisi file, maka file dalam folder tersebut tidak dihiraukan, cukup file pada 1 level saja.

Setiap 1 file yang dikategorikan dioperasikan oleh 1 thread agar bisa berjalan secara paralel sehingga proses kategori bisa berjalan lebih cepat. Program ini dilarang juga menggunakan fork-exec dan system.

***kategori.c***

[kodingan](https://github.com/notdevi/SoalShiftSISOP20_modul3_C10/blob/master/soal3/kategori.c)

**PENJELASAN :**

Deklarasi terlebih dahulu `pthread_t tid[100]` serta `char wdasal[1024]`

Lalu membuat fungsi untuk membedakan directory dengan file biasa
```c
int cekfile(const char *path) {
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);
}
```

Ini merupakan fungsi untuk memindahkan file dari direktori a ke direktori tempat file dituju, jadi digunakan saat menjalankan perintah `-d`.
```c
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
```

`strtok` disini berfungsi untuk memisahkan string. Dengan menggunakan template dari internet, kita bisa mendapatkan ekstensi dari sebuah file yang ingin dikategorikan.
```c
	split1 = strtok(path, ".");
	while(split1 != NULL) {
		titik[n] = split1;
		n++;
		split1 = strtok(NULL, ".");
	}
```

Dikarenakan disoal tidak berlaku case sensitive, jadi JPG dan jpg adalah sama kita menggunakan `tolower` untuk menconvert huruf kapital menjadi huruf kecil. Serta jika file tidak mempuyai ekstensi maka nanti akan membuat folder dengan nama "Unknown".
```c
	if(n != 1) {
		int a;
		for(a = 0; a < strlen(titik[n-1]); a++) {
			format[a] = tolower(titik[n-1][a]);
		}
	}
	else if (n == 1) {
		strcpy(format, "Unknown");
	}
```

Untuk mendapatkan nama file yang diinginkan menggunakan,
```c
	split2 = strtok(b, "/");
	while(split2 != NULL) {
		slash[n] = split2;
		n++;
		split2 = strtok(NULL, "/");
	}
	strcpy(namafile, slash[n-1]);
```

Selanjutnya untuk mengambil alamat cwd serta proses untuk memindahkan file dalam bahasa c menggunakan,
```c
	strcpy(dirname, wdasal);
	strcat(dirname, "/");
	strcat(dirname, format);

	memset(format, 0, 100);
	mkdir(dirname, 0777);

	FILE *psrc, *ptjn;
	//psrc itu source
	//ptjn itu tujuan
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
	//memindahkan file di c
	while ((ch = fgetc(psrc)) != EOF) {
		fputc(ch, ptjn);
	}
	fclose(psrc);		//menutup file yang ditunjuk pointer psrc
	fclose(ptjn);		//menutup file yang ditunjuk pointer ptjn
	remove(a);
```

Sedangkan untuk fungsi `pindah` disini untuk mengecek serta memindahkan file.
```c
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
```

Untuk penjelasan kedua fungsi diatas sama saja strukturnya. Sebenarnya fungsi `pindah` dan `pindahd` bisa digabung, namun karena dua command `-f` dan `\*` itu bekerja di direktori sendiri, sedangkan untuk comman `-d` itu bekerja di beda direktori. Perbedaannya pada fungsi `pindahd` terdapat `char wdasal[1024]` yang dideclare sebagai variabel global, sedangkan padafungsi `pindah` terdapat `char wdtujuan[1024]` yang dideclare didalam fungsi itu sendiri.

Selanjutnya masuk ke program driver untuk menguji fungsi di atas
```c
int main(int argc, char *argv[]) {

	getcwd(wdasal, sizeof(wdasal));

	// if(getcwd(wdasal, sizeof(wdasal)) != NULL) {
	// 	printf("%s\n", wdasal);		//untuk ngeprint directorynya
	// }

	int i;
	// jika command yang dimasukkan -f
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
	// jika command yang dimasukkan \*
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
			while((d = readdir(dr)) != NULL){	//mengambil file dan direktori sampai tidak null
				if(cekfile(d->d_name)) {	//diseleksi lagi untuk yang diambil nanti hanya file saja
					strcpy(subdir, workd);
					strcat(subdir, "/");
					strcat(subdir, d->d_name);
					pthread_create(&(tid[i]), NULL, pindah, (void *)subdir); //membuat thread
					pthread_join(tid[i], NULL);				 //join thread
					i++;
				}
			}
			closedir(dr);
		}
	}
	// jika command yang dimasukkan -d
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
					pthread_create(&(tid[i]), NULL, pindahd, (void *)subdir);  //membuat thread
					pthread_join(tid[i], NULL);				   //join thread
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
```

### Soal No. 4

(a) Buatlah program C dengan nama "4a.c", yang berisi program untuk melakukan perkalian matriks. Ukuran matriks pertama adalah 4 x 2, dan matriks kedua 2 x 5. Isi dari matriks didefinisikan di dalam kodingan. Matriks nantinya akan berisi angka 1-20 (tidak perlu dibuat filter angka). Tampilkan matriks hasil perkalian tadi ke layar.

***soal4a.c***

[kodingan](https://github.com/notdevi/SoalShiftSISOP20_modul3_C10/blob/master/soal4/soal4a.c)

**PENJELASAN :**

Hal pertama yang dilakukan adalah mendefinisikan ukuran matriks dan mengisi elemen-elemennya dengan angka 1-20. Ukuran matriks di definisikan dengan syntax `#define`.

Tiap-tiap matriks direpresentasikan sebagai variable global menggunakan `array` 2 dimensi bernama `matA` dan `matB`, sedangkan `matC` merupakan matriks hasil perkalian.
```c
#define N 4
#define M 2
#define K 5
#define MAX_THREADS N * K

int matA[N][M] = {{1, 2}, {2, 3}, {3, 4}, {4, 5}};
int matB[M][K] = {{1, 3, 5, 7, 9}, {2, 4, 6, 8, 10}};
int matC[N][K];
```
Kemudian dibuat sebuah `struct` bernama `val` yang berisi baris dan kolom dari matriks yang digunakan untuk mem-passing data ke threads.
```c
struct val {
	int i, j;
};
```
Kemudian dibuat sebuah fungsi thread `multp` yang melakukan operasi perkalian pada elemen-elemen matriks. 
```c
void *multp(void *arg) {
	struct val *data = arg;
	int sum = 0;

	for(int i=0; i<M; i++) {
		sum += matA[data->i][i] * matB[i][data->j];
	}

	matC[data->i][data->j] = sum;
	pthread_exit(0);
}
```
Untuk melakukan casting parameter ke pointer struct v digunakan `struct val *data = arg;`. Lalu inisiasi nilai hasil perkalian `int sum = 0;`.
Kemudian hasil perkalian dalam variable `sum` di assign sebagai elemen dari `matC` dengan indeks yang bersesuaian yaitu `i` dan `j`.

Dalam fungsi main, pertama kita lakukan deklarasi variable thread sebanyak `MAX_THREADS` yang telah di declare yaitu bernilai `N * K = 20` yang merupakan perkalian jumlah baris `matA` dengan jumlah kolom `matB`.
```c
pthread_t threads[MAX_THREADS];
```
Dimana `MAX_THREAD` merupakan banyaknya operasi thread tersebut dibuat dan dijalankan untuk melakukan operasi perkalian elemen-elemen matriks.

Kemudian dilakukan loop untuk pembuatan threads sebanyak 20.
```c
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
```
Didalam loop terdapat fungsi `pthread_create` untuk membuat thread dan mem-passing data sebagai parameter. Serta `pthread_join` untuk menunggu thread selesai. 

Kemudian dibuat loop untuk menampilkan hasil dari perkalian yaitu elemen-elemen matriks `matC`.
```c
	for(int i=0; i<N; i++) {
		for(int j=0; j<K; j++) {
			test[l] = matC[i][j];
			l++;
			printf("%d\t", matC[i][j]);

		}
		printf("\n");
	}
```

(b) Buatlah program C dengan nama "4b.c". Program akan mengambil variabel hasil perkalian matriks dari program "4a.c" (program sebelumnya), dan menampilkan hasil matriks tersebut ke layar. Selanjutnya lakukan operasi penjumlahan 1 hingga n dari elemen matriks dan tampilkan ke layar dengan format seperti matriks.

***soal4b.c***

[kodingan](https://github.com/notdevi/SoalShiftSISOP20_modul3_C10/blob/master/soal4/soal4b.c)

**PENJELASAN :**

Agar matriks hasil proses soal4a dapat diakses, maka digunakan shared memory.

Untuk itu, pada kodingan soal a telah dilakukan pendeklarasian unique key `key_t key = 1500;`. Pada kodingan soal a juga dilakukan proses `write` array ke dalam shared memory.
```c
	int sizemem = sizeof(*test);
	int shmid = shmget(key, 20, IPC_CREAT | 0666);

	test = (int *)shmat(shmid, 0, 0);
```
Pada variable `shmid` disimpan sebuah identifier untuk segmen shared memory yang didapat dari fungsi `shmget(key, 20, IPC_CREAT | 0666)`. Agar bisa digunakan, maka segmen shared memory perlu di attach ke alamat dari proses dengan menggunakan fungsi `shmat(shmid, 0, 0)`.

Dalam loop print matriks pada program sebelumnya, juga dibuat sebuah array 1 dimensi baru bernama `test` yang diisi dengan elemen-elemen matriks `matC`.
```c
			test[l] = matC[i][j];
			l++;
```
Kemudian program melakukan detachment ketika telah selesai melakukan segmen shared memory dengan menggunakan fungsi `shmdt((void *) test);`.

Pada kodingan soal4b, dilakukan bagian me-read data dari shared memory yang telah di write oleh program soal4a.
Prosesnya kurang lebih sama dengan proses write, perbedaannya adalah tidak ada pengalokasian memori pada proses read.
```c
	int shmid = shmget(key, 20, IPC_CREAT | 0666);

	test = (int *)shmat(shmid, 0, 0);
	
	...
	
	shmdt((void *) test);
	shmctl(shmid, IPC_RMID, NULL);
	
```

Untuk melakukan operasi penjumlahan, dibuat fungsi thread `penjumlahan` yang melakukan operasi penjumlahan dari elemen-elemen matriks sekaligus mencetak hasilnya. Pada fungsi ini, digunakan rumus `val = (((val+1)*val)/2);` dimana val adalah elemen matriks yang hendak dihitung.
```c
void *penjumlahan(void* arg) {
	struct number* angka;
	angka = (struct number *) arg;

	ll val = angka->value;
	val = (((val+1)*val)/2);
	
	printf("%llu\t", val);
}
```
Pada fungsi `main` dilakukan loop pembuatan thread sebanyak jumlah operasi yang dilakukan terhadap elemen yaitu 20. 
```c
for(int i=0; i<MAX_THREADS; i++) {
		struct number* angka = (struct number*)malloc(sizeof(struct number));
		angka->value = pindah[i];

		pthread_create(&threads[i], NULL, penjumlahan, (void *)angka);
		pthread_join(threads[i], NULL);
		
		if((i%5) == 4) {
			printf("\n");
		}	
	}
```
Kemudian untuk menunggu thread selesai
```c
	for(int i=0; i<MAX_THREADS; i++) {
		pthread_join(threads[i], NULL);
	}
```

(c) Buatlah program untuk mengetahui jumlah file dan folder pada direktori saat ini dengan command `ls | wc -l` dengan menggunakan IPC.

***soal4c.c***

[kodingan](https://github.com/notdevi/SoalShiftSISOP20_modul3_C10/blob/master/soal4/soal4c.c)

**PENJELASAN :**
karena hanya terdapat 1 buah pipe, maka dilakukan 1 kali fork untuk membuat 1 parent dan 1 child.
```c
child_id = fork();
	if(child_id < 0) {
		exit(EXIT_FAILURE);
	}
```
Didalam child, dijalankan proses pemanggilan command `ls` yang akan me-list semua file dan folder pada current working directory.
```c
	if(child_id == 0) {
		close(1);
		dup(fd[1]);
		close(fd[0]);
		char *list[] = {"ls", NULL};
		execv("/bin/ls", list);
	}
```
Kemudian parent menunggu proses child selesai dengan `while((wait(&status)) > 0);`. Lalu menjalankan proses pemanggilan command `wc -l`.
```c
 else {
		while((wait(&status)) > 0);
		close(0);
		dup(fd[0]);
		close(fd[1]);
		char *count[] = {"wc", "-l", NULL};
		execv("/usr/bin/wc", count);
	}
```
