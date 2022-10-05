#include <unistd.h>

typedef struct torta
{
    char* descricao;
    short int qualidade;
    short int doce;
    useconds_t tempo_producao;
    useconds_t tempo_consumo;
} Torta;
