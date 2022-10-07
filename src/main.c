/**BEGIN REGION: Importações**/
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
#include "./models/dados_consumidor.h"
#include "./models/fila.h"
/**END REGION: Importações**/


/**BEGIN REGION: Constantes**/
#define N_CONSUMIDORES 2
#define N_MAX 100000
#define CONSTANTE_PARSE_SEG 1000000
/**END REGION: Constantes**/

/**BEGIN REGION: Variáveis globais**/
Queue fila;

sem_t mutex_fila;
sem_t sem_consumidor[N_CONSUMIDORES];

int index_torta_salgada = 0;
int index_torta_doce = 0;

Torta tortas_doces[N_MAX];
Torta tortas_salgadas[N_MAX];

char *tipos_tortas[] = {"Torta Chocolate", "Torta Amendoim", "Torta Frango"};

pthread_t thread[5];
/**END REGION: Variáveis globais**/

/**BEGIN REGION: Métodos Produtor.**/
void adicionar_torta_fila(Torta torta) {
    sem_wait(&mutex_fila);
    enqueue(&fila, &torta);
    sem_post(&mutex_fila);
}

void produzir_torta(struct dados_produtor* dados) {
    int tempo_producao = (rand() % 5) + 2;

    Torta torta = {
        .serial = (rand() % 20000) * (rand() % 20),
        .descricao = tipos_tortas[dados->tipo_torta],
        .doce = dados->tipo_torta_doce,
        .qualidade = rand() % 3,
        .tempo_producao = tempo_producao,
        .tempo_consumo = tempo_producao * 0.5
    };

    printf("[Produtor %d] Comecei produção da torta de serial %d.\n\t Descrição da Torta: %s;\n\t Qualidade: %d;\n\t Tempo Consumo: %d;\n\t Tempo Produção: %d;\n", dados->id_produtor, torta.serial, torta.descricao, torta.qualidade, torta.tempo_consumo, torta.tempo_producao);
    usleep(torta.tempo_producao * CONSTANTE_PARSE_SEG);
    printf("[Produtor %d] Finalizei a produção torta de serial %d.\n", dados->id_produtor, torta.serial);
    adicionar_torta_fila(torta);  
    sem_post(&sem_consumidor[dados->sem_index_consumidor]);
}

/**END REGION: Métodos Produtor.**/

/**BEGIN REGION: Métodos Consumidor.**/
int mandar_para_outro_consumidor(int processa_doce) {
    Torta torta_atual;

    sem_wait(&mutex_fila);

    if (getQueueSize(&fila) == 0) {
        sem_post(&mutex_fila);
        return 0;
    }

    queuePeek(&fila, &torta_atual);

    sem_post(&mutex_fila);

    return torta_atual.doce != processa_doce;
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

void adicionar_torta_lista(Torta torta) {
    if (torta.doce) {
        tortas_doces[index_torta_doce] = torta;
        index_torta_doce ++;
    } else {
        tortas_salgadas[index_torta_salgada] = torta;
        index_torta_salgada ++;
    }
}

void consumir_tortas(struct dados_consumidor* dados) {
    sem_wait(&sem_consumidor[dados->sem_index_consumidor]);

    if (mandar_para_outro_consumidor(dados->processa_torta_doce)) {
        sem_post(&sem_consumidor[dados->sem_index_consumidor_concorrente]);
        return;
    }

    Torta torta_consumida = obter_torta_disponivel();

    if (torta_consumida.descricao == NULL) {
        return;
    }

    printf("[Consumidor %d] Comecei a consumir torta de serial: %d.\n\t Descrição da Torta: %s;\n\t Qualidade: %d;\n\t Tempo Consumo: %d;\n\t Tempo Produção: %d;\n", dados->id_consumidor, torta_consumida.serial, torta_consumida.descricao, torta_consumida.qualidade, torta_consumida.tempo_consumo, torta_consumida.tempo_producao);
    usleep(torta_consumida.tempo_consumo * CONSTANTE_PARSE_SEG);
    printf("[Consumidor %d] Finalizei o consumo da torta de serial: %d.\n", dados->id_consumidor, torta_consumida.serial);

    adicionar_torta_lista(torta_consumida);
}
/**END REGION: Métodos Consumidor.**/

/**BEGIN REGION: Inicializadores.**/
void *criar_produtor(void * dados_produtor) {
    struct dados_produtor *dados_prod = (struct dados_produtor*) dados_produtor;

    for (int i = 0; i < dados_prod->quantidade_producao; i++) {
        produzir_torta(dados_prod);
    }

    pthread_exit(0);
}

void *criar_consumidor(void * dados_consumidor) {
    while (1) {
        consumir_tortas((struct dados_consumidor*) dados_consumidor);
    }
}
/**END REGION: Inicializadores.**/

/**BEGIN REGION: Finalização programa.**/
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

        printf("Torta %d: %s com qualidade %d com tempo de produção %d seg. e tempo de consumo %d seg.\n", torta.serial, torta.descricao, torta.qualidade, torta.tempo_producao, torta.tempo_consumo);
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

        printf("Torta %d: %s com qualidade %d com tempo de produção %d seg. e tempo de consumo %d seg.\n", torta.serial, torta.descricao, torta.qualidade, torta.tempo_producao, torta.tempo_consumo);
    }

    printf("\nMédia de tempo de produção: %f\n", media_tempo_producao / quantidade_tortas_doce);
    printf("Média de tempo de consumo: %f\n", media_tempo_consumo / quantidade_tortas_doce);
    printf("Média de qualidade: %f\n", media_qualidade / quantidade_tortas_doce);

    printf("\n--------------------------------------------------------------------------------------------\n\t Tortas Desperdiçadas \n--------------------------------------------------------------------------------------------\n");
    
    for (int i = 0; i < quantidade_tortas_desperdicadas; i++) {
        Torta torta;
        dequeue(&fila, &torta);
        printf("Torta %d: %s com qualidade %d com tempo de produção %d seg. e tempo de consumo %d seg.\n", torta.serial, torta.descricao, torta.qualidade, torta.tempo_producao, torta.tempo_consumo);
    }
    
    printf("Quantidade total de desperdício: %d\n\n", quantidade_tortas_desperdicadas);

    exit(EXIT_SUCCESS);
}

void finalizar_programa() {
    printf("\nProdução finalizada.\n");

    for(int i = 0; i < 5; i++) {
        pthread_cancel(thread[i]);
    }

    gerar_relatorios();

    exit(0);
}
/**BEGIN REGION: Finalização programa.**/

void inicializar_semaforos() {
    sem_init(&mutex_fila, 0, 1);
    sem_init(&sem_consumidor[0], 0, 0);
    sem_init(&sem_consumidor[1], 0, 0);
}

void inicializar_signal() {
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = &finalizar_programa;

    sigaction(SIGINT, &sa, NULL);
}

void inicializar_produtores(int quantidade_tortas_produtor) {
    struct dados_produtor dados_produtor = {
        .id_produtor = 1,
        .sem_index_consumidor = 0,
        .tipo_torta = 0,
        .tipo_torta_doce = 1,
        .quantidade_producao = quantidade_tortas_produtor
    };

    struct dados_produtor dados_produtor2 = {
        .id_produtor = 2,
        .sem_index_consumidor = 0,
        .tipo_torta = 1,
        .tipo_torta_doce = 1,
        .quantidade_producao = quantidade_tortas_produtor
    };

    struct dados_produtor dados_produtor3 = {
        .id_produtor = 3,
        .sem_index_consumidor = 1,
        .tipo_torta = 2,
        .tipo_torta_doce = 0,
        .quantidade_producao = quantidade_tortas_produtor
    };

    pthread_create(&thread[0], NULL, criar_produtor, (void*) &dados_produtor);
    pthread_create(&thread[1], NULL, criar_produtor, (void*) &dados_produtor2);
    pthread_create(&thread[2], NULL, criar_produtor, (void*) &dados_produtor3);
}

void inicializar_consumidores() {
    struct dados_consumidor dados_cosumidor1 = {
        .id_consumidor = 1,
        .processa_torta_doce = 1,
        .sem_index_consumidor = 0,
        .sem_index_consumidor_concorrente = 1
    };

    struct dados_consumidor dados_cosumidor2 = {
        .id_consumidor = 2,
        .processa_torta_doce = 0,
        .sem_index_consumidor = 1,
        .sem_index_consumidor_concorrente = 0
    };

    pthread_create(&thread[3], NULL, criar_consumidor, (void*) &dados_cosumidor1);
    pthread_create(&thread[4], NULL, criar_consumidor, (void*) &dados_cosumidor2);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Uso correto: ./trabalho %s <quantidade_tortas_produtor> \n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int quantidade_tortas_produtor = atol(argv[1]);
    
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = &finalizar_programa;

    sigaction(SIGINT, &sa, NULL);

    srand(time(NULL));

    sem_init(&mutex_fila, 0, 1);
    sem_init(&sem_consumidor[0], 0, 0);
    sem_init(&sem_consumidor[1], 0, 0);

    queueInit(&fila, sizeof(Torta));

    struct dados_produtor dados_produtor = {
        .id_produtor = 1,
        .sem_index_consumidor = 0,
        .tipo_torta = 0,
        .tipo_torta_doce = 1,
        .quantidade_producao = quantidade_tortas_produtor
    };

    struct dados_produtor dados_produtor2 = {
        .id_produtor = 2,
        .sem_index_consumidor = 0,
        .tipo_torta = 1,
        .tipo_torta_doce = 1,
        .quantidade_producao = quantidade_tortas_produtor
    };

    struct dados_produtor dados_produtor3 = {
        .id_produtor = 3,
        .sem_index_consumidor = 1,
        .tipo_torta = 2,
        .tipo_torta_doce = 0,
        .quantidade_producao = quantidade_tortas_produtor
    };

    struct dados_consumidor dados_cosumidor1 = {
        .id_consumidor = 1,
        .processa_torta_doce = 1,
        .sem_index_consumidor = 0,
        .sem_index_consumidor_concorrente = 1
    };

    struct dados_consumidor dados_cosumidor2 = {
        .id_consumidor = 2,
        .processa_torta_doce = 0,
        .sem_index_consumidor = 1,
        .sem_index_consumidor_concorrente = 0
    };

    pthread_create(&thread[0], NULL, criar_produtor, (void*) &dados_produtor);
    pthread_create(&thread[1], NULL, criar_produtor, (void*) &dados_produtor2);
    pthread_create(&thread[2], NULL, criar_produtor, (void*) &dados_produtor3);
    pthread_create(&thread[3], NULL, criar_consumidor, (void*) &dados_cosumidor1);
    pthread_create(&thread[4], NULL, criar_consumidor, (void*) &dados_cosumidor2);

    (void) pthread_join(thread[0], NULL);
    (void) pthread_join(thread[1], NULL);
    (void) pthread_join(thread[2], NULL);
    (void) pthread_join(thread[3], NULL);
    (void) pthread_join(thread[4], NULL);

    exit(EXIT_SUCCESS);
}