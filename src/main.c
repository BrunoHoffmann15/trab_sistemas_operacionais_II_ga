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

void adicionar_torta_fila(Torta torta) {
    sem_wait(&mutex);
    enqueue(&fila, &torta);
    sem_post(&mutex);
}

void produzir_torta(struct dados_produtor* dados) {
    printf("[Produtor %d] Comecei fluxo.\n", dados->tipo_torta);
    int tempo_producao = rand() % 10;

    Torta torta = {
        .descricao = tipos_tortas[dados->tipo_torta],
        .doce = dados->tipo_torta_doce,
        .qualidade = rand() % 10,
        .tempo_producao = tempo_producao,
        .tempo_consumo = tempo_producao * 0.5
    };

    adicionar_torta_fila(torta);  
    
    printf("[Produtor %d] Enfileirei torta.\n", dados->tipo_torta);
    sem_post(&sem_consumidor[dados->sem_index_consumidor]);
    usleep(torta.tempo_producao * 1000000);
}

Torta obter_torta_disponivel() {
    Torta torta_atual;

    sem_wait(&mutex);
    
    if (getQueueSize(&fila) == 0) {
        sem_post(&mutex);
        return torta_atual;
    }

    dequeue(&fila, &torta_atual);
    sem_post(&mutex);

    return torta_atual;
}

void consumir_tortas_doces() {
    sem_wait(&sem_consumidor[0]);
    printf("Doce - Vou pegar da fila.\n");

    Torta torta_consumida = obter_torta_disponivel();

    printf("Doce - Consegui pegar torta, vou consumir: %d\n", torta_consumida.tempo_consumo);

    if (torta_consumida.doce == 0) {
        printf("Doce - Não é meu tipo de torta");
        sem_post(&sem_consumidor[1]);
        return;
    }

    if (torta_consumida.descricao == NULL) {
        printf("Doce - estou sem coisa na fila.\n");
        return;
    }

    usleep(torta_consumida.tempo_consumo * 1000000);
    printf("Doce - consumi %s\n", torta_consumida.descricao);
}

void consumir_tortas_salgadas() {
    sem_wait(&sem_consumidor[1]);
    printf("Salgado - Vou pegar da fila.\n");

    Torta torta_consumida = obter_torta_disponivel();

    printf("Doce - Consegui pegar torta, vou consumir: %d\n", torta_consumida.tempo_consumo);

    if (torta_consumida.doce == 0) {
        printf("Salgado - Não é meu tipo de torta");
        sem_post(&sem_consumidor[0]);
        return;
    }

    if (torta_consumida.descricao == NULL) {
        printf("Salgado - estou sem coisa na fila.\n");
        return;
    }

    usleep(torta_consumida.tempo_consumo * 1000000);
    printf("Salgado - consumi %s\n", torta_consumida.descricao);
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

void *criar_consumidor2(void * dados_consumidor) {
    while (1) {
        consumir_tortas_salgadas();
    }
}

int main() {
    pthread_t thread[5];

    sem_init(&mutex, 0, 1);
    sem_init(&sem_consumidor[0], 0, 0);
    sem_init(&sem_consumidor[1], 0, 0);

    queueInit(&fila, sizeof(Torta));

    struct dados_produtor dados = {
        .sem_index_consumidor = 0,
        .tipo_torta = 0,
        .tipo_torta_doce = 1
    };

    struct dados_produtor dados2 = {
        .sem_index_consumidor = 0,
        .tipo_torta = 1,
        .tipo_torta_doce = 1
    };

    struct dados_produtor dados3 = {
        .sem_index_consumidor = 1,
        .tipo_torta = 2,
        .tipo_torta_doce = 1
    };

    pthread_create(&thread[0], NULL, criar_produtor, (void*) &dados);
    pthread_create(&thread[1], NULL, criar_produtor, (void*) &dados2);
    pthread_create(&thread[2], NULL, criar_produtor, (void*) &dados3);
    pthread_create(&thread[3], NULL, criar_consumidor, NULL);
    pthread_create(&thread[4], NULL, criar_consumidor2, NULL);

    (void) pthread_join(thread[0], NULL);
    (void) pthread_join(thread[1], NULL);
    (void) pthread_join(thread[2], NULL);
    (void) pthread_join(thread[3], NULL);
    (void) pthread_join(thread[4], NULL);

}