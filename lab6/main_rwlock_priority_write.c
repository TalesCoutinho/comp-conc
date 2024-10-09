//Programa concorrente que cria e faz operacoes sobre uma lista de inteiros
#include <stdio.h>
#include <stdlib.h>
#include "list_int.h"
#include <pthread.h>
#include "timer.h"

#define QTDE_OPS 10000000 //quantidade de operacoes sobre a lista (insercao, remocao, consulta)
#define QTDE_INI 100 //quantidade de insercoes iniciais na lista
#define MAX_VALUE 100 //valor maximo a ser inserido

//lista compartilhada iniciada 
struct list_node_s* head_p = NULL; 
//qtde de threads no programa
int nthreads;

//rwlock de exclusao mutua
pthread_rwlock_t rwlock;
// Variável para dar prioridade à escrita
int write_requests = 0; 

//tarefa das threads
void* tarefa(void* arg) {
   long int id = (long int) arg;
   int op;
   int in, out, read; 
   in=out=read = 0; 

   //realiza operacoes de consulta (98%), insercao (1%) e remocao (1%)
   for(long int i=id; i<QTDE_OPS; i+=nthreads) {
      op = rand() % 100;
      
      if(op<98) {  // Leitura
         pthread_rwlock_rdlock(&rwlock);  /* lock de LEITURA */    
         if(write_requests > 0) { // Checa se há solicitações de escrita pendentes
            pthread_rwlock_unlock(&rwlock); // Libera o lock e aguarda a escrita
            pthread_rwlock_rdlock(&rwlock); // Re-adquire o lock após a escrita
         }
         Member(i%MAX_VALUE, head_p);   /* Ignore return value */
         pthread_rwlock_unlock(&rwlock); /* Unlock de leitura */
         read++;
      } else if (op < 99) {  // Remoção
         __sync_fetch_and_add(&write_requests, 1); // Incrementa o número de pedidos de escrita
         pthread_rwlock_wrlock(&rwlock);  /* lock de ESCRITA */
         Delete(i%MAX_VALUE, &head_p);    /* Remove da lista */
         pthread_rwlock_unlock(&rwlock);  /* Unlock de escrita */
         __sync_fetch_and_sub(&write_requests, 1); // Decrementa o número de pedidos de escrita
         out++;  // Incrementa o número de remoções
      } else {  // Inserção
         __sync_fetch_and_add(&write_requests, 1);
         pthread_rwlock_wrlock(&rwlock);  /* lock de ESCRITA */
         Insert(i%MAX_VALUE, &head_p);   /* Insere na lista */
         pthread_rwlock_unlock(&rwlock);  /* Unlock de escrita */
         __sync_fetch_and_sub(&write_requests, 1);
         in++;
      }
   }
   printf("Thread %ld finalizou com %d insercoes, %d remocoes, %d leituras\n", id, in, out, read);
   return NULL;
}

int main(int argc, char* argv[]) {
   pthread_t* threads;
   long thread;
   int i;
   double ini, fim;

   if (argc != 2) {
      printf("Use: %s <numero de threads>\n", argv[0]);
      exit(0);
   }

   nthreads = strtol(argv[1], NULL, 10);
   threads = malloc(nthreads*sizeof(pthread_t));

   pthread_rwlock_init(&rwlock, NULL);

   //inicia a lista encadeada com valores aleatorios
   for(i=0; i<QTDE_INI; i++)
      Insert(rand()%MAX_VALUE, &head_p);

   GET_TIME(ini);

   //cria as threads 
   for(thread=0; thread<nthreads; thread++)  
      pthread_create(&threads[thread], NULL, tarefa, (void*) thread);

   //aguarda as threads finalizarem
   for(thread=0; thread<nthreads; thread++) 
      pthread_join(threads[thread], NULL);

   GET_TIME(fim);

   printf("Tempo de execucao: %lf\n", fim-ini);

   pthread_rwlock_destroy(&rwlock);
   free(threads);
   return 0;
}