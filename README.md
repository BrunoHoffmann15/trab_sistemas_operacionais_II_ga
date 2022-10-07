# Trabalho de Análise e Aplicação de Sistemas Operacionais (Grau A)

Aluno: Bruno da Siqueira Hoffmann

Disciplina: Análise e Aplicação de Sistemas Operacionais.

## Contexto do Trabalho
O presente trabalho represente uma fábrica de produção de tortas, onde há máquinas produtoras, máquinas consumidoras e uma esteira. As tortas podem ser classificadas entre doces e salgadas, e os consumidores serão responsável por consumir um tipo de torta específica. 

As tortas possuíram os seguintes atributos:
- Serial: um identificador.
- Qualidade: um valor relacionado a sua qualidade (0 - Ruim; 1 - Bom; 2 - Muito bom;).
- Tipo da torta: se ela é doce ou salgada
- Descrição: qual o nome da torta.
- Tempo de produção e consumo.

## Explicação da Estrutura do Trabalho

Abaixo é dado uma breve explicação da estrutura de pastas e arquivos do projeto:

- `src/main.c` possui a lógica central da aplicação;
- `src/models/fila.h` possui a definição da fila;
- `src/models/fila.c` possui a implementação da fila;
- `src/models/torta.h` possui a definição do objeto torta;
- `src/models/dados_produtor.h` possui a definição do objeto genérico do produtor;
- `src/models/dados_consumidor.h` possui a definição do objeto genérico do consumidor;
- `docs/` possui as apresentações e algumas documentações do trabalho.

## Explicação da Implementação

A implementação consiste no uso de signals, threads, semphores e queues.

- `queue`: foi a estrutura de dados escolhida com o intuito de fazer a representação da esteira.

- `signals`: responsável por finalizar o programa e gerar os relatórios com as estatísticas da execução.


- `threads`: possibilita realizar a execução concorrente das diferentes partes do código. Para o caso do trabalho, foi gerado 3 threads para os produtores e 2 para os consumidores.

-  `semaphores`: possibilitam controle do acesso a zona crítica do código, além de fazer o controle da condicional de execução dos consumidores. No caso, foram criados 3 semáforos, um deles responsável pelo acesso a fila, os outros dois responsáveis pelo controle de poder ou não executar os consumidores.

> Obs: Para o controle dos consumidores, há um semáforo específico para cada um, que fará com que ele espere sua vez de executar. Esse é liberado pelo próprio produtor que colocar informação na fila, ou pelo consumidor concorrente que não pode consumir a torta.

## Como executar

Para executar o presente trabalho é necessário possuir o compilador C em sua máquina. Assim executar o comando abaixo:

```shell
gcc ./main.c ./models/fila.c -o trabalhoGA -lpthread
```

Após, é possível executar o comando abaixo, passando como parâmetro a quantidade de tortas que se deseja produzir por produtor.

```shell
./trabalhoGA 3
```

Para finalizar o programa e gerar o relatório basta executar o comando Control + C.
```shell
^C
```

### Requisitos de Execução

A implementação e a execução do trabalho foi feita dentro uma *WSL* Ubuntu, dessa forma é recomendado a execução em uma distro linux. Além de, ter visto algumas limitações na execução do mesmo no MacOS, onde para a biblioteca `lpthreads` haviam limitações de métodos, que foram utilizados no trabalho.

## Requisitos do Trabalho

1. Organização da apresentação oral de como o projeto foi concebido – valor 1.
2. Definição e criação da estrutura dos produtos e atributos – valor 1.
3. Criação dos processos de entrada com a atribuição dos seus dados – valor 1.
4. Definição e criação da estrutura da esteira – valor 1.
5. Controle da esteira e dos tempos dos processos – 1.
6. Controle dos produtos através de sincronização de processos – valor 1.
7. Apresentação do algoritmo de controle da seleção dos produtos – valor 1.
8. Visualização dos produtos criados e transmitidos – valor 1.
9. Visualização dos produtos processados – valor 1.
10. Apresentação estatística das informações processadas – valor 1.

## Lista de Tarefas
[x] Escolher tema para trabalho.

[x] Criar estrutura do projeto.

[x] Criar estrutura do objeto e propriedades.

[x] Criar fluxo de controle de entrada.

[x] Criar estrutura da esteira.

[x] Criar estrutura da saída.

[x] Criar apresentação.

[x] Gerar relatório de execução.

## Referências

- A implementação da fila foi retirada do site: https://codereview.stackexchange.com/questions/141238/implementing-a-generic-queue-in-c