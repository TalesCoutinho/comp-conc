#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

long int soma = 0; // Variável compartilhada entre as threads
int count_multiples = 0; // Contador dos múltiplos de 10 impressos
int printed_soma_value = 1; // Flag para indicar se o múltiplo atual foi impresso (1 = sim, 0 = não)
pthread_mutex_t mutex; // Variável de lock para exclusão mútua
pthread_cond_t cond_reached_multiple; // Variável de condição para sinalizar múltiplo de 10 alcançado
pthread_cond_t cond_printed; // Variável de condição para sinalizar que o valor foi impresso

// Função executada pelas threads de incremento
void *ExecutaTarefa (void *arg) {
  long int id = (long int) arg;
  printf("Thread : %ld está executando...\n", id);

  for (int i = 0; i < 100000; i++) {
    pthread_mutex_lock(&mutex);
    soma++; // Incrementa a variável compartilhada

    // Se 'soma' é múltiplo de 10 e ainda não foram impressos 20 múltiplos
    if (soma % 10 == 0 && count_multiples < 20) {
      // Se o valor atual ainda não foi sinalizado para impressão
      if (printed_soma_value == 1) {
        printed_soma_value = 0; // Indica que o valor precisa ser impresso
        pthread_cond_signal(&cond_reached_multiple); // Sinaliza a thread extra
      }
      // Aguarda até que o valor seja impresso
      while (printed_soma_value == 0 && count_multiples < 20) {
        pthread_cond_wait(&cond_printed, &mutex);
      }
    }

    pthread_mutex_unlock(&mutex);
  }
  printf("Thread : %ld terminou!\n", id);
  pthread_exit(NULL);
}

// Função executada pela thread extra
void *extra (void *args) {
  printf("Extra : está executando...\n");

  while (1) {
    pthread_mutex_lock(&mutex);

    // Verifica se já foram impressos 20 múltiplos
    if (count_multiples >= 20) {
      pthread_mutex_unlock(&mutex);
      printf("Extra : terminou!\n");
      pthread_exit(NULL);
    }

    // Aguarda até que 'printed_soma_value' seja 0 (múltiplo de 10 não impresso)
    while (printed_soma_value == 1) {
      // Verifica novamente para evitar deadlocks
      if (count_multiples >= 20) {
        pthread_mutex_unlock(&mutex);
        printf("Extra : terminou!\n");
        pthread_exit(NULL);
      }
      pthread_cond_wait(&cond_reached_multiple, &mutex);
    }

    // Imprime o valor de 'soma'
    printf("soma = %ld\n", soma);
    count_multiples++;
    printed_soma_value = 1; // Indica que o valor foi impresso

    pthread_cond_broadcast(&cond_printed); // Acorda as threads de incremento

    pthread_mutex_unlock(&mutex);
  }
}

// Fluxo principal
int main(int argc, char *argv[]) {
  pthread_t *tid; // Identificadores das threads no sistema
  int nthreads; // Qtde de threads (passada na linha de comando)

  // Lê e avalia os parâmetros de entrada
  if (argc < 2) {
    printf("Digite: %s <número de threads>\n", argv[0]);
    return 1;
  }
  nthreads = atoi(argv[1]);

  // Aloca as estruturas
  tid = (pthread_t*) malloc(sizeof(pthread_t)*(nthreads+1));
  if (tid == NULL) {
    puts("ERRO--malloc");
    return 2;
  }

  // Inicializa o mutex e as variáveis de condição
  pthread_mutex_init(&mutex, NULL);
  pthread_cond_init(&cond_reached_multiple, NULL);
  pthread_cond_init(&cond_printed, NULL);

  // Cria as threads de incremento
  for (long int t = 0; t < nthreads; t++) {
    if (pthread_create(&tid[t], NULL, ExecutaTarefa, (void *)t)) {
      printf("--ERRO: pthread_create()\n");
      exit(-1);
    }
  }

  // Cria a thread extra
  if (pthread_create(&tid[nthreads], NULL, extra, NULL)) {
    printf("--ERRO: pthread_create()\n");
    exit(-1);
  }

  // Espera todas as threads terminarem
  for (int t = 0; t < nthreads + 1; t++) {
    if (pthread_join(tid[t], NULL)) {
      printf("--ERRO: pthread_join()\n");
      exit(-1);
    }
  }

  // Finaliza o mutex e as variáveis de condição
  pthread_mutex_destroy(&mutex);
  pthread_cond_destroy(&cond_reached_multiple);
  pthread_cond_destroy(&cond_printed);

  printf("Valor final de 'soma' = %ld\n", soma);
  printf("Total de múltiplos de 10 impressos: %d\n", count_multiples);

  return 0;
}
