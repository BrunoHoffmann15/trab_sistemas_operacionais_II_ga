#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <semaphore.h>
#include <signal.h>
#include <string.h>

#include "./models/torta.h"
#include "./models/dados_produtor.h"
#include "./models/fila.h"

#define N_CONSUMIDORES 2
#define N_MAX 100000
#define CONSTANTE_PARSE_SEG 1000000

Queue fila;

sem_t mutex_fila;
sem_t sem_consumidor[N_CONSUMIDORES];
sem_t mutex_consumidores;

int index_torta_salgada = 0;
int index_torta_doce = 0;

Torta tortas_doces[N_MAX];
Torta tortas_salgadas[N_MAX];

char *tipos_tortas[] = {"Torta Chocolate", "Torta Amendoim", "Torta Frango"};

pthread_t thread[5];

void adicionar_torta_fila(Torta torta) {
    sem_wait(&mutex_fila);
    enqueue(&fila, &torta);
    sem_post(&mutex_fila);
}

void produzir_torta(struct dados_produtor* dados) {
    printf("[Produtor %d] Comecei fluxo.\n", dados->id_produtor);
    int tempo_producao = (rand() % 5) + 2;

    Torta torta = {
        .descricao = tipos_tortas[dados->tipo_torta],
        .doce = dados->tipo_torta_doce,
        .qualidade = rand() % 3,
        .tempo_producao = tempo_producao,
        .tempo_consumo = tempo_producao * 0.5
    };

    printf("[Produtor %d] Comecei produção da torta.\n", dados->id_produtor);
    usleep(torta.tempo_producao * CONSTANTE_PARSE_SEG);
    adicionar_torta_fila(torta);  
    sem_post(&sem_consumidor[dados->sem_index_consumidor]);
    printf("[Produtor %d] Finalizei a produção torta.\n", dados->id_produtor);
}

int mandar_para_outro_consumidor(int doce) {
    Torta torta_atual;

    sem_wait(&mutex_fila);

    if (getQueueSize(&fila) == 0) {
        sem_post(&mutex_fila);
        return 0;
    }

    queuePeek(&fila, &torta_atual);

    sem_post(&mutex_fila);

    return torta_atual.doce != doce;
}

Torta obter_torta_disponivel() {
    Torta torta_atual;

    sem_wait(&mutex_fila);
    
    if (getQueueSize(&fila) == 0) {
        sem_post(&mutex_fila);
        return torta_atual;
    }

    dequeue(&fila, &torta_atual);
    sem_post(&mutex_fila);

    return torta_atual;
}

void consumir_tortas_doces() {
    sem_wait(&sem_consumidor[0]);
    printf("Doce - Vou pegar da fila.\n");

    if (mandar_para_outro_consumidor(1)) {
        printf("Doce - Não é meu tipo de torta\n");
        sem_post(&sem_consumidor[1]);
        return;
    }

    Torta torta_consumida = obter_torta_disponivel();

    printf("Doce - Consegui pegar torta, vou consumir: %d\n", torta_consumida.tempo_consumo);

    if (torta_consumida.descricao == NULL) {
        printf("Doce - estou sem coisa na fila.\n");
        return;
    }

    usleep(torta_consumida.tempo_consumo * CONSTANTE_PARSE_SEG);

    tortas_doces[index_torta_doce] = torta_consumida;
    index_torta_doce ++;

    printf("Doce - consumi %s\n", torta_consumida.descricao);
}

void consumir_tortas_salgadas() {
    sem_wait(&sem_consumidor[1]);
    printf("Salgado - Vou pegar da fila.\n");

    if (mandar_para_outro_consumidor(0)) {
        printf("Salgado - Não é meu tipo de torta\n");
        sem_post(&sem_consumidor[0]);
        return;
    }

    Torta torta_consumida = obter_torta_disponivel();

    printf("Doce - Consegui pegar torta, vou consumir: %d\n", torta_consumida.tempo_consumo);

    usleep(torta_consumida.tempo_consumo * CONSTANTE_PARSE_SEG);

    tortas_salgadas[index_torta_salgada] = torta_consumida;
    index_torta_salgada ++;
    
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

void gerar_relatorios() {
    int quantidade_tortas_doce = index_torta_doce;
    int quantidade_tortas_salgada = index_torta_salgada;
    int quantidade_tortas_desperdicadas = getQueueSize(&fila);
    float media_tempo_producao = 0;
    float media_tempo_consumo = 0;
    float media_qualidade = 0;

    printf("\n--------------------------------------------------------------------------------------------\n\t Estatistica da Produção de Tortas \n--------------------------------------------------------------------------------------------\n");
    printf("Quantidade total de tortas prontas: %d\n", quantidade_tortas_doce + quantidade_tortas_salgada);
    printf("Quantidade de tortas doces prontas: %d\n", quantidade_tortas_doce);
    printf("Quantidade de tortas salgadas prontas: %d\n", quantidade_tortas_salgada);
    printf("Quantidade de tortas desperdiçadas: %d\n", quantidade_tortas_desperdicadas);

    printf("\n--------------------------------------------------------------------------------------------\n\t Tortas Doces Produzidas \n--------------------------------------------------------------------------------------------\n");
    
    for (int i = 0; i < index_torta_doce; i++) {
        Torta torta = tortas_doces[i];

        media_tempo_producao += torta.tempo_producao;
        media_tempo_consumo += torta.tempo_consumo;
        media_qualidade += torta.qualidade;

        printf("Torta %d: %s com qualidade %d com tempo de produção %d seg. e tempo de consumo %d seg.\n", i + 1, torta.descricao, torta.qualidade, torta.tempo_producao, torta.tempo_consumo);
    }

    printf("\nMédia de tempo de produção: %f\n", media_tempo_producao / quantidade_tortas_doce);
    printf("Média de tempo de consumo: %f\n", media_tempo_consumo / quantidade_tortas_doce);
    printf("Média de qualidade: %f\n", media_qualidade / quantidade_tortas_doce);

    media_tempo_consumo = media_qualidade = media_tempo_producao = 0;

    printf("\n--------------------------------------------------------------------------------------------\n\t Tortas Salgadas Produzidas \n--------------------------------------------------------------------------------------------\n");
    
    for (int i = 0; i < index_torta_salgada; i++) {
        Torta torta = tortas_salgadas[i];

        media_tempo_producao += torta.tempo_producao;
        media_tempo_consumo += torta.tempo_consumo;
        media_qualidade += torta.qualidade;

        printf("Torta %d: %s com qualidade %d com tempo de produção %d seg. e tempo de consumo %d seg.\n", i + 1, torta.descricao, torta.qualidade, torta.tempo_producao, torta.tempo_consumo);
    }

    printf("\nMédia de tempo de produção: %f\n", media_tempo_producao / quantidade_tortas_doce);
    printf("Média de tempo de consumo: %f\n", media_tempo_consumo / quantidade_tortas_doce);
    printf("Média de qualidade: %f\n", media_qualidade / quantidade_tortas_doce);

    printf("\n--------------------------------------------------------------------------------------------\n\t Tortas Desperdiçadas \n--------------------------------------------------------------------------------------------\n");
    for (int i = 0; i < quantidade_tortas_desperdicadas; i++) {
        Torta torta;
        dequeue(&fila, &torta);
        printf("Torta %d: %s com qualidade %d com tempo de produção %d seg. e tempo de consumo %d seg.\n", i + 1, torta.descricao, torta.qualidade, torta.tempo_producao, torta.tempo_consumo);
    }

    exit(EXIT_SUCCESS);
}

void finalizar_programa() {
    printf("Produção finalizada.\n");

    for(int i = 0; i < 5; i++) {
        pthread_cancel(thread[i]);
    }

    gerar_relatorios();

    exit(0);
}

int main() {
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = &finalizar_programa;

    sigaction(SIGINT, &sa, NULL);

    srand(time(NULL));

    sem_init(&mutex_fila, 0, 1);
    sem_init(&sem_consumidor[0], 0, 0);
    sem_init(&sem_consumidor[1], 0, 0);
    sem_init(&mutex_consumidores, 0, 1);

    queueInit(&fila, sizeof(Torta));

    struct dados_produtor dados = {
        .id_produtor = 1,
        .sem_index_consumidor = 0,
        .tipo_torta = 0,
        .tipo_torta_doce = 1
    };

    struct dados_produtor dados2 = {
        .id_produtor = 2,
        .sem_index_consumidor = 0,
        .tipo_torta = 1,
        .tipo_torta_doce = 1
    };

    struct dados_produtor dados3 = {
        .id_produtor = 3,
        .sem_index_consumidor = 1,
        .tipo_torta = 2,
        .tipo_torta_doce = 0
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
