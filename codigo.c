#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define M 2			// Numero de productores
#define N 2			// Numero de consumidores
#define TAM 5			// Capacidad del buffer
#define DATOS 10		// Datos que producira cada productor

// Buffer compartido
int buf[TAM];
int rear = 0;
int front = 0;

// Semaforos
sem_t empty;
sem_t full;

// Mutex
pthread_mutex_t mutexP;
pthread_mutex_t mutexC;

// Prototipos Productor y Consumidor
void *Producer(void *arg);
void *Consumer(void *arg);

// Funcion principal
int main() {

	pthread_t productores[M];
	pthread_t consumidores[N];
	int idProductor[M] = {1, 2};
	int idConsumidor[N] = {1, 2};

	// Inicializar semaforos
	sem_init(&empty, 0, TAM);
	sem_init(&full, 0, 0);

	// Inicializar mutex
	pthread_mutex_init(&mutexP, NULL);
	pthread_mutex_init(&mutexC, NULL);

	printf("=== PROBLEMA PRODUCTOR-CONSUMIDOR ===\n");
	printf("Productores: %d\n", M);
	printf("Consumidores: %d\n", N);
	printf("Capacidad del buffer: %d\n\n", TAM);

	// Crear productores
	for (int i = 0; i < M; i++) {
		pthread_create(&productores[i], NULL, Producer, &idProductor[i]);
	}

	// Crear consumidores
	for (int i = 0; i < N; i++) {
		pthread_create(&consumidores[i], NULL, Consumer, &idConsumidor[i]);
	}

	// Esperar a que terminen los productores
	for (int i = 0; i < M; i++) {
		pthread_join(productores[i], NULL);
	}

	// Esperar a que terminen los consumidores
	for (int i = 0; i < N; i++) {
		pthread_join(consumidores[i], NULL);
	}

	// Destruir semaforos y mutex
	sem_destroy(&empty);
	sem_destroy(&full);
	pthread_mutex_destroy(&mutexP);
	pthread_mutex_destroy(&mutexC);

	printf("\n=== PROGRAMA FINALIZADO ===\n");
	return 0;
}

// Funcion Productor
void *Producer(void *arg) {

	int id = *(int *)arg;
	int data;

	for (int i = 0; i < DATOS; i++) {

		// Producir dato
		data = id * 100 + i;
		printf("Productor %d produjo: %d\n", id, data);

		sem_wait(&empty);				// P(empty)
		pthread_mutex_lock(&mutexP);		// P(mutexP)

		// Insertar dato en el buffer
		buf[rear] = data;
		printf("Productor %d inserto: %d en posicion %d\n", id, data, rear);
		rear = (rear + 1) % TAM;

		pthread_mutex_unlock(&mutexP);	// V(mutexP)
		sem_post(&full);				// V(full)
		//usleep(100000);
    }
    return NULL;
}

// Funcion Consumidor
void *Consumer(void *arg) {

	int id = *(int *)arg;
	int result;

	for (int i = 0; i < DATOS; i++) {

		sem_wait(&full);// P(full)
		pthread_mutex_lock(&mutexC);// P(mutexC)

		// Extraer dato del buffer
		result = buf[front];
		printf("Consumidor %d extrajo: %d de posicion %d\n", id, result, front);
		front = (front + 1) % TAM;

		pthread_mutex_unlock(&mutexC);						// V(mutexC)
		sem_post(&empty);									// V(empty)
		printf("Consumidor %d proceso: %d\n", id, result);		// Consumir dato
		//usleep(150000);
	}
	return NULL;
}
