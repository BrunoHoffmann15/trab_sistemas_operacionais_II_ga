// TODO: Criar estrutura consumidor.
// TODO: Criar estrutura fila.
// TODO: Criar estrutura produtor.
// TODO: Criar estrutura objeto.
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <semaphore.h>

#include "./models/torta.h"
#include "./models/dados_produtor.h"
#include "./models/fila.h"

#define N_CONSUMIDORES 2

Queue fila;

sem_t mutex;
sem_t sem_consumidor[N_CONSUMIDORES];

char *tipos_tortas[] = {"Torta Chocolate", "Torta Amendoim", "Torta Frango"};

void adicionar_torta_fila(struct torta torta) {
    sem_wait(&mutex);
    enqueue(&fila, &torta);
    sem_post(&mutex);
}

void produzir_torta(struct dados_produtor* dados) {
    printf("Produzindo torta.\n");
    
    Torta torta = {
        .descricao = tipos_tortas[dados->tipo_torta],
        .doce = dados->tipo_torta_doce,
        .qualidade = rand() % 10
    };

    adicionar_torta_fila(torta);  
    
    printf("Enfileirei torta.\n");
    sem_post(&sem_consumidor[dados->sem_index_consumidor]);
    usleep(1000000 * (rand() % 10));
}

Torta *obter_torta_disponivel() {
    Torta *torta_atual;

    sem_wait(&mutex);
    
    if (getQueueSize(&fila) == 0) {
        printf("tá vazia essa merda\n");
        sem_post(&mutex);
        return NULL;
    }

    dequeue(&fila, &torta_atual);
    sem_post(&mutex);

    return torta_atual;
}

void consumir_tortas_doces() {
    usleep(1000000 * 3);
    sem_wait(&sem_consumidor[0]);
    printf("Doce - Vou pegar da fila.\n");

    Torta *torta_atual = obter_torta_disponivel();

    if (torta_atual == NULL) {
        printf("estou sem coisa na fila.\n");
        sem_post(&sem_consumidor[0]);
        return;
    }

    printf("consumi\n");
    sem_post(&sem_consumidor[0]);
}

void *criar_produtor(void * dados_produtor) {
    while (1) {
        produzir_torta((struct dados_produtor*) dados_produtor);
    }
}

void *criar_consumidor(void * dados_consumidor) {
    while (1) {
        consumir_tortas_doces();
    }
}


int main() {
    pthread_t thread[5];

    sem_init(&mutex, 0, 1);
    sem_init(&sem_consumidor[0], 0, 0);
    sem_init(&sem_consumidor[1], 0, 0);

    struct dados_produtor dados = {
        .sem_index_consumidor = 0,
        .tipo_torta = 0,
        .tipo_torta_doce = 1
    };

    pthread_create(&thread[0], NULL, criar_produtor, (void*) &dados);
    pthread_create(&thread[1], NULL, criar_consumidor, NULL);
    //pthread_create(&thread[2], NULL, criar_consumidor2, NULL);

    (void) pthread_join(thread[0], NULL);
    (void) pthread_join(thread[1], NULL);
    (void) pthread_join(thread[2], NULL);
}