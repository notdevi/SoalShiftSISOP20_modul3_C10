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
```

Selanjutnya masuk ke program driver untuk menguji fungsi di atas
```c
int main(int argc, char const *argv[]){
    int x;
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
