#include <stdio.h>
#include <pthread.h>
#define NTHREADS 5

// Variáveis globais
int chegadas = 0;
int sentados = 0;
pthread_mutex_t x_mutex;
pthread_cond_t chegada_cond;
pthread_cond_t sentado_cond;

void *chegada(void *arg) {
    int thread_id = *(int*)arg;
    printf("Thread de chegada %d iniciou.\n", thread_id);

    if (thread_id == 0)
        printf("Oi José!\n");
    else
        printf("Oi Maria!\n");

    pthread_mutex_lock(&x_mutex);
    chegadas++;
    printf("Thread de chegada %d incrementou 'chegadas' para %d.\n", thread_id, chegadas);
    if (chegadas == 2) {
        printf("Thread de chegada %d sinalizando 'chegada_cond'.\n", thread_id);
        pthread_cond_signal(&chegada_cond);
    }
    pthread_mutex_unlock(&x_mutex);

    printf("Thread de chegada %d finalizou.\n", thread_id);
    pthread_exit(NULL);
}

void *permanencia(void *arg) {
    printf("Thread 'permanencia' iniciou.\n");

    pthread_mutex_lock(&x_mutex);
    while (chegadas != 2) {
       printf("Thread 'permanencia' aguardando 'chegada_cond'. 'chegadas' = %d.\n", chegadas);
       pthread_cond_wait(&chegada_cond, &x_mutex);
    }
    printf("Thread 'permanencia' prosseguindo. 'chegadas' = %d.\n", chegadas);
    printf("Sentem-se por favor.\n");
    sentados++;
    printf("Thread 'permanencia' incrementou 'sentados' para %d.\n", sentados);
    printf("Thread 'permanencia' realizando 'pthread_cond_broadcast' em 'sentado_cond'.\n");
    pthread_cond_broadcast(&sentado_cond);
    pthread_mutex_unlock(&x_mutex);

    printf("Thread 'permanencia' finalizou.\n");
    pthread_exit(NULL);
}

void *saida(void *arg) {
    int thread_id = *(int*)arg;
    printf("Thread de saída %d iniciou.\n", thread_id);

    pthread_mutex_lock(&x_mutex);
    while(sentados != 1) {
       printf("Thread de saída %d aguardando 'sentado_cond'. 'sentados' = %d.\n", thread_id, sentados);
       pthread_cond_wait(&sentado_cond, &x_mutex);
    }
    pthread_mutex_unlock(&x_mutex);

    if (thread_id == 3)
        printf("Tchau José!\n");
    else
        printf("Tchau Maria!\n");

    printf("Thread de saída %d finalizou.\n", thread_id);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    pthread_t threads[NTHREADS];
    int thread_ids[NTHREADS] = {0, 1, 2, 3, 4};

    // Inicializa o mutex e as variáveis de condição
    pthread_mutex_init(&x_mutex, NULL);
    pthread_cond_init(&chegada_cond, NULL);
    pthread_cond_init(&sentado_cond, NULL);

    printf("Criando as threads...\n");

    // Cria as threads
    pthread_create(&threads[0], NULL, chegada, &thread_ids[0]);
    pthread_create(&threads[1], NULL, chegada, &thread_ids[1]);
    pthread_create(&threads[2], NULL, permanencia, &thread_ids[2]);
    pthread_create(&threads[3], NULL, saida, &thread_ids[3]);
    pthread_create(&threads[4], NULL, saida, &thread_ids[4]);

    // Aguarda as threads terminarem
    for (int i = 0; i < NTHREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Programa finalizado.\n");

    // Destrói o mutex e as variáveis de condição
    pthread_mutex_destroy(&x_mutex);
    pthread_cond_destroy(&chegada_cond);
    pthread_cond_destroy(&sentado_cond);

    return 0;
}
