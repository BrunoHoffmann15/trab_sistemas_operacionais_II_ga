#ifndef TORTA_H_INCLUDED
#define TORTA_H_INCLUDED

#include <unistd.h>

typedef struct {
    char* descricao;
    short int qualidade;
    short int doce;
    useconds_t tempo_producao;
    useconds_t tempo_consumo;
} Torta;

#endif /* QUEUE_H_INCLUDED */
