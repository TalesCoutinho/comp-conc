#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "timer.h"

float *mat;    // matriz de entrada
float *result; // matriz resultante da multiplicação
int nthreads;  // número de threads

typedef struct {
    int id;  // identificador da thread
    int dim; // dimensão da matriz
} tArgs;

// Função que as threads irão executar
void *tarefa(void *arg) {
    tArgs *args = (tArgs *)arg;
    int dim = args->dim;

    // Multiplicação da matriz por ela mesma
    for (int i = args->id; i < dim; i += nthreads) {
        for (int j = 0; j < dim; j++) {
            result[i * dim + j] = 0;
            for (int k = 0; k < dim; k++) {
                result[i * dim + j] += mat[i * dim + k] * mat[k * dim + j];
            }
        }
    }

    pthread_exit(NULL);
}

// Função para ler a matriz de um arquivo binário
void lerMatrizBinaria(char *arquivo, int dim) {
    FILE *fp = fopen(arquivo, "rb");
    if (fp == NULL) {
        fprintf(stderr, "Erro ao abrir o arquivo de entrada.\n");
        exit(1);
    }
    fread(mat, sizeof(float), dim * dim, fp);
    fclose(fp);
}

// Função para salvar a matriz resultante em um arquivo binário
void salvarMatrizBinaria(char *arquivo, int dim) {
    FILE *fp = fopen(arquivo, "wb");
    if (fp == NULL) {
        fprintf(stderr, "Erro ao abrir o arquivo de saída.\n");
        exit(1);
    }
    fwrite(result, sizeof(float), dim * dim, fp);
    fclose(fp);
}

int main(int argc, char *argv[]) {
    int dim;          // dimensão da matriz
    pthread_t *tid;   // identificadores das threads
    tArgs *args;      // argumentos para as threads
    double inicio, fim, tempoTotal = 0, mediaTempo;

    // Leitura e avaliação dos parâmetros de entrada
    if (argc < 4) {
        printf("Uso: %s <arquivo_entrada> <arquivo_saida> <numero_de_threads>\n", argv[0]);
        return 1;
    }

    char *arquivoEntrada = argv[1];
    char *arquivoSaida = argv[2];
    nthreads = atoi(argv[3]);

    // Leitura da dimensão da matriz (deve ser conhecida)
    printf("Digite a dimensão da matriz: ");
    scanf("%d", &dim);

    // Ajusta o número de threads se for maior que a dimensão
    if (nthreads > dim)
        nthreads = dim;

    // Alocação de memória para as matrizes
    mat = (float *)malloc(sizeof(float) * dim * dim);
    if (mat == NULL) {
        printf("ERRO--malloc\n");
        return 2;
    }
    result = (float *)malloc(sizeof(float) * dim * dim);
    if (result == NULL) {
        printf("ERRO--malloc\n");
        return 2;
    }

    // Leitura da matriz de entrada do arquivo binário
    lerMatrizBinaria(arquivoEntrada, dim);

    // Alocação das estruturas
    tid = (pthread_t *)malloc(sizeof(pthread_t) * nthreads);
    if (tid == NULL) {
        printf("ERRO--malloc\n");
        return 2;
    }
    args = (tArgs *)malloc(sizeof(tArgs) * nthreads);
    if (args == NULL) {
        printf("ERRO--malloc\n");
        return 2;
    }

    // Executa a multiplicação 5 vezes para calcular a média dos tempos
    for (int iter = 0; iter < 5; iter++) {
        // Criação das threads
        GET_TIME(inicio);
        for (int i = 0; i < nthreads; i++) {
            (args + i)->id = i;
            (args + i)->dim = dim;
            if (pthread_create(tid + i, NULL, tarefa, (void *)(args + i))) {
                printf("ERRO--pthread_create\n");
                return 3;
            }
        }

        // Espera pelo término das threads
        for (int i = 0; i < nthreads; i++) {
            pthread_join(*(tid + i), NULL);
        }
        GET_TIME(fim);

        // Calcula o tempo da iteração e acumula
        double tempoExecucao = fim - inicio;
        tempoTotal += tempoExecucao;
        printf("Tempo da execução %d: %e segundos\n", iter + 1, tempoExecucao);
    }

    // Calcula a média dos tempos
    mediaTempo = tempoTotal / 5;
    printf("Tempo médio de execução: %e segundos\n", mediaTempo);

    // Salva a matriz resultante no arquivo binário de saída
    salvarMatrizBinaria(arquivoSaida, dim);

    // Liberação da memória
    free(mat);
    free(result);
    free(args);
    free(tid);

    return 0;
}
