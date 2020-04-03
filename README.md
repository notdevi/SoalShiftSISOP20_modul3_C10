# SoalShiftSISOP20_modul3_C10
Praktikum Modul 3 Sistem Operasi 2020

### Nama Anggota Kelompok :
### 1. Devi Hainun Pasya (05111840000014)
### 2. Anggara Yuda Pratama (05111840000008)

### Soal No. 1

### Soal No. 2

### Soal No. 3

Buatlah sebuah program C untuk mengkategorikan file. Program memindahkan file sesuai dengan ekstensinya (tidak case sensitive) ke dalam folder sesuai ekstensinya yang folder hasilnya terdapat di working directory ketika program tersebut dijalankan.

Untuk opsi `-f` user bisa menambahkan argumen file yang bisa dikategorikan sebanyak yang diinginkan.

Untuk opsi `-d` digunakan untuk melakukan kategori pasa suatu direktori.

**PENJELASAN :**

***soal3.c***

[kodingan](https://github.com/notdevi/SoalShiftSISOP20_modul3_C10/blob/master/soal3/soal3.c)

**PENJELASAN :**

Deklarasi terlebih dahulu `pthread_t tid[100]'

Lalu membuat fungsi untuk membedakan directory dengan file biasa
```c
int cekfile(const char *path) {
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);
}
```

Ini merupakan fungsi untuk melakukan command2 yang disuruh di int main
```c
void *pindah(void *arg) {
	char ext[100], filename[100];
	char *pisah1, *pisah2, *titik[10], *slash[10], path[100];
	int i = 0, j = 0;
	
	strcpy(path, arg);
	pisah1 = strtok(path, ".");		//strtok untuk memisahkan string
	while(pisah1 != NULL) {
		titik[i] = pisah1;
		pisah1 = strtok(NULL, ".");
		i++;
	}
	
	//mengambil ekstensi dari file
	printf("|-%s\n", titik[i-1]);
	strcpy(ext, titik[i-1]);

	pisah2 = strtok(path, "/");
	while(pisah2 != NULL) {
		slash[j] = pisah2;
		pisah2 = strtok(NULL, "/");
		j++;
	}
	
	//mengambil nama file
	printf("|--%s\n", slash[j-1]);
	strcpy(filename, slash[j-1]);
	
	//???
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
```

Selanjutnya masuk ke program driver untuk menguji fungsi di atas
```c
int main(int argc, char const *argv[]){
	if(getcwd(workingdir, sizeof(workingdir)) !=  NULL) {
		printf("%s\n", workingdir);
	}
	
	//untuk menjalankan perintah -f
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
	//untuk menjalankan peritah -d
	else if(strcmp("-d", argv[1]) == 0 && argc == 3) {
		
	}
	//untuk menjalankana perintah *
	else if(strcmp("*", argv[1]) == 0 && argc == 2) {
        
	} 
	//untuk argument yang tidak sesuai perintah
	else {
		printf("Invalid Arguments\n");
	}
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
