#include <stdio.h>
#include <stdlib.h>
#include "timer.h"
#include <time.h>

float *mat;    // Matriz de entrada
float *result; // Matriz resultante da multiplicação

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

// Função sequencial para multiplicar a matriz por ela mesma
void multiplicarMatriz(int dim) {
    for (int i = 0; i < dim; i++) {
        for (int j = 0; j < dim; j++) {
            result[i * dim + j] = 0;
            for (int k = 0; k < dim; k++) {
                result[i * dim + j] += mat[i * dim + k] * mat[k * dim + j];
            }
        }
    }
}

int main(int argc, char *argv[]) {
    int dim;          // Dimensão da matriz
    double inicio, fim, tempoTotal = 0, mediaTempo;

    // Leitura e avaliação dos parâmetros de entrada
    if (argc < 3) {
        printf("Uso: %s <arquivo_entrada> <arquivo_saida>\n", argv[0]);
        return 1;
    }

    char *arquivoEntrada = argv[1];
    char *arquivoSaida = argv[2];

    // Leitura da dimensão da matriz (deve ser conhecida)
    printf("Digite a dimensão da matriz: ");
    scanf("%d", &dim);

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

    // Executa a multiplicação 5 vezes para calcular a média dos tempos
    for (int iter = 0; iter < 5; iter++) {
        // Medição do tempo de execução da multiplicação
        GET_TIME(inicio);
        multiplicarMatriz(dim);
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

    return 0;
}
