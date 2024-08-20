/* Disciplina: Programacao Concorrente */
/* Profa.: Silvana Rossetto */
/* Laboratório: 1 */
/* Código: Incremento de elementos de um vetor usando threads em C */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

// Cria a estrutura de dados para armazenar os argumentos da thread
typedef struct {
    int idThread, nThreads;
    int start, end;
    int *array;
} t_Args;

// Função executada pelas threads
void *increment(void *arg) {
    t_Args *args = (t_Args *) arg;

    for (int i = args->start; i < args->end; i++) {
        args->array[i] += 1;
    }

    free(arg); // Libera a alocação feita na main
    pthread_exit(NULL);
}

// Função para inicializar o vetor com valores de 0 a N-1
void initializeArray(int *array, int N) {
    for (int i = 0; i < N; i++) {
        array[i] = i;
    }
}

// Função para verificar se o vetor está correto (todos os elementos devem ser i+1)
int checkArray(int *array, int N) {
    for (int i = 0; i < N; i++) {
        if (array[i] != i + 1) {
            return 0;
        }
    }
    return 1;
}


// Função principal do programa
int main(int argc, char* argv[]) {
    int N, M; // Número de elementos do vetor e número de threads
    t_Args *args; // Receberá os argumentos para as threads

    // Verifica se os argumentos foram passados e armazena seus valores
    if (argc < 3) {
        printf("--ERRO: informe o número de elementos e o número de threads <%s> <N> <M>\n", argv[0]);
        return 1;
    }
    N = atoi(argv[1]);
    M = atoi(argv[2]);

    // Identificadores das threads no sistema
    pthread_t tid_sistema[M];

    // Aloca o vetor
    int *array = (int *)malloc(N * sizeof(int));
    if (array == NULL) {
        printf("--ERRO: malloc() para o vetor\n");
        return 1;
    }

    // Inicializa o vetor
    initializeArray(array, N);

    // Inicia a contagem de tempo
    clock_t start_time = clock();

    // Cria as threads
    int elementsPerThread = N / M;
    int remainder = N % M;

    for (int i = 0; i < M; i++) {
        printf("--Aloca e preenche argumentos para thread %d\n", i + 1);
        args = malloc(sizeof(t_Args));
        if (args == NULL) {
            printf("--ERRO: malloc()\n");
            return 1;
        }
        args->idThread = i + 1;
        args->nThreads = M;
        args->start = i * elementsPerThread;
        args->end = (i + 1) * elementsPerThread;
        if (i == M - 1) {
            args->end += remainder; // A última thread pega os elementos restantes
        }
        args->array = array;

        printf("--Cria a thread %d\n", i + 1);
        if (pthread_create(&tid_sistema[i], NULL, increment, (void*) args)) {
            printf("--ERRO: pthread_create()\n");
            return 2;
        }
    }

    // Espera todas as threads terminarem
    for (int i = 0; i < M; i++) {
        if (pthread_join(tid_sistema[i], NULL)) {
            printf("--ERRO: pthread_join() da thread %d\n", i + 1);
        }
    }

    // Para a contagem de tempo
    clock_t end_time = clock();


    // Verifica se o vetor foi corretamente incrementado
    if (checkArray(array, N)) {
        printf("Vetor incrementado corretamente.\n");
    } else {
        printf("Erro ao incrementar o vetor.\n");
    }

    // Calcula e imprime o tempo de execução
    double time_taken = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    printf("Tempo de execução: %f segundos\n", time_taken);

    // Libera a memória alocada
    free(array);

    // Log da função principal
    printf("--Thread principal terminou\n");

    return 0;
}
