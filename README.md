# SoalShiftSISOP20_modul3_C10
Praktikum Modul 3 Sistem Operasi 2020

### Nama Anggota Kelompok :
### 1. Devi Hainun Pasya (05111840000014)
### 2. Anggara Yuda Pratama (05111840000008)

### Soal No. 1

### Soal No. 2

### Soal No. 3


***soal3.c***

[kodingan]()

**PENJELASAN :**

### Soal No. 4


***soal4.c***

[kodingan]()

**PENJELASAN :**

(a) Buatlah program C dengan nama "4a.c", yang berisi program untuk melakukan perkalian matriks. Ukuran matriks pertama adalah 4 x 2, dan matriks kedua 2 x 5. Isi dari matriks didefinisikan di dalam kodingan. Matriks nantinya akan berisi angka 1-20 (tidak perlu dibuat filter angka). Tampilkan matriks hasil perkalian tadi ke layar.

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






