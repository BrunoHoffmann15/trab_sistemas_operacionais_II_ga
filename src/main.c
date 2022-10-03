// TODO: Criar estrutura consumidor.
// TODO: Criar estrutura fila.
// TODO: Criar estrutura produtor.
// TODO: Criar estrutura objeto.

#include "./models/torta.h";
#include "./models/fila.h";

Queue fila;

void produzir_torta_chocolate() {
    Torta torta = {
        .descricao = "Torta Chocolate",
        .doce = 1,
        .qualidade = rand() % 10
    };

    enqueue(fila, torta);
}

void produzir_torta_amendoim() {
    Torta torta = {
        .descricao = "Torta Amendoim",
        .doce = 1,
        .qualidade = rand() % 10
    };

    enqueue(fila, torta);
}

void produzir_torta_frango() {
    Torta torta = {
        .descricao = "Torta Frango",
        .doce = 0,
        .qualidade = rand() % 10
    };

    enqueue(fila, torta);
}

void consumir_tortas_salgadas() {
    Torta torta_atual;

    queuePeek(fila, &torta_atual);

    if (torta_atual.doce) {
        // libera consumidor dos doces.
    }

    dequeue(fila, &torta_atual);
    // separa consumo.   
}

void consumir_tortas_doces() {
    Torta torta_atual;

    queuePeek(fila, &torta_atual);

    if (torta_atual.doce) {
        // libera consumidor dos doces.
    }

    dequeue(fila, &torta_atual);
    // separa consumo.
}

int main() {

}