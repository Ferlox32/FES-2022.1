/* Disciplina: Computacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Módulo 1 - Laboratório: 1 */
/* Codigo: "Hello World" usando threads em C passando mais de um argumento */

#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>

#define NTHREADS  2 //total de threads a serem criadas

int vetor[10000];

//cria a estrutura de dados para armazenar os argumentos da thread
typedef struct {
   int runs;
} t_Args;

//funcao executada pelas threads
void *PrintHello (void *arg) {

  t_Args *args = (t_Args *) arg;
  int i;
  
  if(args -> runs == 0){
      
      for(i = 0; i < 5000; i++){
          vetor[i] = vetor[i] * vetor[i];
      }
      
  }else{
      
      for(i = 5000; i < 10000; i++){
          vetor[i] = vetor[i] * vetor[i];
      }
      
  }
  printf("Thread %d\n", args->runs);
  
  free(arg); //aqui pode liberar a alocacao feita na main

  pthread_exit(NULL);
}

//funcao principal do programa
int main() {
  pthread_t tid_sistema[NTHREADS]; //identificadores das threads no sistema
  int thread; //variavel auxiliar
  t_Args *arg; //receberá os argumentos para a thread

  for(thread=0; thread<NTHREADS; thread++) {
    printf("--Aloca e preenche argumentos para thread %d\n", thread);
    arg = malloc(sizeof(t_Args));
    if (arg == NULL) {
      printf("--ERRO: malloc()\n"); exit(-1);
    }
    arg->runs = thread; 
    for(int i = 0; i < 10000; i++){
        vetor[i] = i;
    }
    
    printf("--Cria a thread %d\n", thread);
    if (pthread_create(&tid_sistema[thread], NULL, PrintHello, (void*) arg)) {
      printf("--ERRO: pthread_create()\n"); exit(-1);
    }
  }

  int x = 0;

  for (thread=0; thread<NTHREADS; thread++) {
    if (pthread_join(tid_sistema[thread], NULL)) {
         printf("--ERRO: pthread_join() \n"); exit(-1); 
         for(int i = 0; i < sizeof vetor; i++){
            if(vetor[i] == i * i){
                continue;
            }else{
                printf("%d = %d?",vetor[i],i);
                x++;
                //printf("Erro!");
            }
        }
    } 
  }

  if(x == 0){
    printf("Certo!");  
  }

  printf("\n\n--Thread principal terminou\n");
  pthread_exit(NULL);
}
