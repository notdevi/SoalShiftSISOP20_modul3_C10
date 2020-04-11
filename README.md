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
