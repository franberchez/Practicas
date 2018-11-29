#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <time.h>

#define HILOS_P 3
#define HILOS_C 2
#define NBUFFER 5

int buffer[NBUFFER]; // aqui es donde actualizo cada producto que produzco o consumo
sem_t mutex_s; // semaforo binario incializado a 1. Para la seccion critica
sem_t n; // semaforo general para el consumidor incializado a 0. Cuenta los espacios ocupados
sem_t e; // semaforo general que se carga con el tamaño de buffer. Para el productor. Cuenta los espacios libres
int indice_p = 0; // indice para el productor
int indice_c = 0; // indice para el consumidor
int suma1 = 0;
int suma2=0;

void *productor(void *p);
void *consumidor(void *p);
void producir(int *dato);
//void consumir(int *suma, int dato);
void consumir(int dato);
void anadir(int dato);
void extraer(int *dato);

int main(){

pthread_t hilos_p[HILOS_P], hilos_c[HILOS_C];

int status, vpadre_p[HILOS_P], vpadre_c[HILOS_C];
int *retorno;
void *productor (void*);
void *consumidor (void*);

time_t t;
srand((int) time(&t));

extern sem_t mutex_s;
extern sem_t n;
extern sem_t e;

	// Creo los semaforos
	sem_init(&mutex_s, 0, 1);
	sem_init(&n, 0, 0);
	sem_init(&e, 0, NBUFFER);
	
	// Creo los hilos
	for (int i=0; i<HILOS_P; i++){
		vpadre_p[i] = i;
		if( status = pthread_create(&hilos_p[i], NULL, productor, (void*) &vpadre_p[i] ) ){
			exit(-1);
		}
	}

	for (int i=0; i<HILOS_C; i++){
		vpadre_c[i] = i;
		if( status = pthread_create(&hilos_c[i], NULL, consumidor, (void*) &vpadre_c[i] ) ){
			exit(-1);
		}
	}


	for (int i=0; i<HILOS_P; i++){
		pthread_join(hilos_p[i], (void**) &retorno);
	}
		printf("\nSuma productor = %d\n", *retorno);

	for (int i=0; i<HILOS_C; i++){
		pthread_join(hilos_c[i], (void**) &retorno);
	}
		printf("\nSuma consumidor = %d\n", *retorno);

return 0;
}

void *productor(void *p){ // los productores hacen la suma de los elementos que producen y las comprueban con las del consumidor
extern sem_t mutex_s;
extern sem_t n;
extern sem_t e;
extern int indice_p;
extern int buffer[NBUFFER];
extern int suma1;

int dato;
//int suma=0;
int *retorno;

	for(int i=0; i<100; i++){

		producir(&dato); //	generar por ejemplo un numero aleatorio
		suma1 = suma1 + dato;
		sem_wait(&e); // un espacio libre menos. Si e<0 me bloqueo
		sem_wait(&mutex_s); // controlo la seccion critica

		anadir(dato); // poner en el buffer (SC) el valor generado
		indice_p++;

		printf("\nBuffer productor: ");
		for (int j=0; j<NBUFFER; j++){
			printf("[%d] ", buffer[j]);
		}

		if ( indice_p == NBUFFER ){
			indice_p = 0;
		}

		sem_post(&mutex_s);
		sem_post(&n); // un espacio mas ocupado por un dato generado

	}

	printf("\n");

	retorno = malloc(sizeof(int));
	*retorno = suma1;
	pthread_exit((void*) retorno);
}

void *consumidor(void *p){ // los consumidores hacen la suma de los elementos que consumen
extern sem_t mutex_s;
extern sem_t n;
extern sem_t e;
extern int indice_c;
extern int buffer[NBUFFER];
extern int suma2;

int *retorno;
//int suma = 0;
int dato;

	for(int i=0; i<150; i++){

		sem_wait(&n); // un espacio menos ocupado, si n<0 me bloqueo
		sem_wait(&mutex_s); //  controlo la seccion critica
		extraer(&dato); // elimina y copia del buffer (SC) un valor
		indice_c++;

		printf("\nBuffer consumidor: ");
		for (int j=0; j<NBUFFER; j++){
			printf("[%d] ", buffer[j]);
		}

		if ( indice_c == NBUFFER  ){
			indice_c = 0;
		}

		sem_post(&mutex_s);
		sem_post(&e); // un espacio libre mas
		//consumir(&suma, dato); // hacer lo que se quiera con el dato. Resto de codigo
		consumir(dato);
	}

	printf("\n");

	retorno = malloc(sizeof(int));
	*retorno = suma2;
	pthread_exit((void*) retorno);
}

void producir(int *dato){
	*dato = rand()%1001; // genera un numero aleatorio entre 0 y 1000
}

/*void consumir(int *suma, int dato){
	*suma = *suma + dato;
}*/

void consumir(int dato){
	extern int suma2;
	suma2 = suma2 + dato;
}


void anadir(int dato){
	extern int buffer[NBUFFER];
	extern int indice_p;
	buffer[indice_p]=dato;

}

void extraer(int *dato){ // voy a definir como quiero ir etrayendo los datos
	extern int buffer[NBUFFER];
	extern int indice_c; // si aqui en vez de sumarle un +1 no se lo sumo lo podria interpretar como que el ultimo elemento que produzco el es primero que saco
	*dato = buffer[indice_c];
	buffer[indice_c]=0; // lo pongo a 0 para indicar que queda vacio
}