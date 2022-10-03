#include <unistd.h>

typedef struct torta
{
    char* descricao;
    short int qualidade;
    short int doce;
    usecond_t tempo_producao;
    usecond_t tempo_consumo;
} Torta;
