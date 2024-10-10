/* Disciplina: Programacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Codigo: As tres threads executam de acordo com as transições de estado (t1-t2-t3) */

#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>
#include <semaphore.h>
#include <string.h>

#define NTHREADS 3
#define MAX_BUFFER 10000

// Variaveis globais
sem_t estado1, estado2;      // Semáforos para coordenar a ordem de execução das threads
char buffer[MAX_BUFFER] = {0};
char aux_buffer[MAX_BUFFER] = {0};
int counter = 1;  // Começa com 1 caractere na primeira linha

// Função executada pela thread 1 (Leitura do arquivo)
void *t1 (void *arg) {
    // Lê o conteúdo do arquivo linha por linha e armazena no buffer
    char temp_buffer[MAX_BUFFER];
    while (fgets(temp_buffer, sizeof(temp_buffer), stdin) != NULL) {
        if (strlen(buffer) + strlen(temp_buffer) < MAX_BUFFER) {
            // Concatena a linha lida ao buffer global, garantindo que não exceda o tamanho do buffer
            strcat(buffer, temp_buffer);
        } else {
            printf("Erro: o buffer excedeu o tamanho máximo permitido.\n");
            break;
        }
    }
    sem_post(&estado1); // Libera a Thread 2 para processar os dados
    pthread_exit(NULL);
}

// Função executada pela thread 2 (Formatação do buffer)
void *t2 (void *arg) {
    sem_wait(&estado1); // Espera a Thread 1 ler os dados
    
    int in_idx = 0, out_idx = 0;
    while (in_idx < strlen(buffer) && out_idx < MAX_BUFFER - 1) {
        // Copia `counter` caracteres da entrada para o aux_buffer
        for (int i = 0; i < counter && in_idx < strlen(buffer); i++) {
            aux_buffer[out_idx++] = buffer[in_idx++];
        }
        aux_buffer[out_idx++] = '\n'; // Adiciona nova linha
        counter += 2; // Incrementa para a próxima linha ter mais 2 caracteres
    }
    
    aux_buffer[out_idx] = '\0'; // Finaliza a string no buffer auxiliar
    sem_post(&estado2); // Libera a Thread 3 para imprimir os dados
    pthread_exit(NULL);
}

// Função executada pela thread 3 (Impressão do buffer)
void *t3 (void *arg) {
    sem_wait(&estado2); // Espera a Thread 2 organizar o buffer
    
    // Imprime o conteúdo formatado do aux_buffer
    printf("%s", aux_buffer);
    
    pthread_exit(NULL);
}

// Função principal
int main(int argc, char *argv[]) {
    pthread_t tid[NTHREADS];

    // Inicializa os semáforos
    sem_init(&estado1, 0, 0);
    sem_init(&estado2, 0, 0);

    // Cria as três threads
    if (pthread_create(&tid[0], NULL, t1, NULL)) { 
        printf("--ERRO: pthread_create()\n"); exit(-1); 
    }
    if (pthread_create(&tid[1], NULL, t2, NULL)) { 
        printf("--ERRO: pthread_create()\n"); exit(-1); 
    }
    if (pthread_create(&tid[2], NULL, t3, NULL)) { 
        printf("--ERRO: pthread_create()\n"); exit(-1); 
    }

    // Espera todas as threads terminarem
    for (int t = 0; t < NTHREADS; t++) {
        if (pthread_join(tid[t], NULL)) {
            printf("--ERRO: pthread_join() \n"); exit(-1); 
        }
    }

    // Destrói os semáforos
    sem_destroy(&estado1);
    sem_destroy(&estado2);

    return 0;
}
