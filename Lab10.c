#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

//A.TODAS OS SEMAFOROS DEVEM SER INICIALIZADOS EM 1
//B.NAO ACHEI UMA MANEIRA DE REDUZIR O NUMERO UTILIZADO

sem_t em_e, em_l, escr, leit;
int e = 0, l = 0;
#define L 6
#define E 3
#define NTHREADS L+E

void* leitores(void* threadid){

    int tid = *(int*) threadid;

    while(1){
        printf("Leitor (%d) quer ler\n", tid);
        sem_wait(&leit);
        sem_wait(&em_l); 
        l++;
        if(l==1) {
            sem_wait(&escr);
        }
        sem_post(&em_l);
        sem_post(&leit);
        printf("Leitor: (%d) esta lendo\n", tid);
        sem_wait(&em_l); 
        l--;
		printf("Leitor: (%d) acabou de ler\n", tid);
        if(l==0){
            sem_post(&escr);
        }
        sem_post(&em_l);
    }
    pthread_exit(NULL);
}

void* escritores(void* threadid){

    int tid = *(int*) threadid;

    while(1){
        printf("Escritor: (%d) quer escrever\n", tid);
        sem_wait(&em_e); 
        e++;
        if(e==1){ 
            sem_wait(&leit);
        }
        sem_post(&em_e);
        sem_wait(&escr);
        printf("Escritor: (%d) esta escrevendo\n", tid);
        sem_post(&escr);
        sem_wait(&em_e); 
        e--;
		printf("Escritor: (%d) acabou de escrever\n", tid);
        if(e==0){ 
            sem_post(&leit);
        }
        sem_post(&em_e);
    }
    pthread_exit(NULL);
}

int main(){
	
	pthread_t tid[L+E];
	int t, id[L+E];
	  
	//inicia os semaforos
	sem_init(&leit, 0, 1);
	sem_init(&escr, 0, 1);
	sem_init(&em_e, 0, 1);
	sem_init(&em_l, 0, 1);

	//cria as threads
	for (t=0; t<E; t++) {
		id[t]=t;
		if (pthread_create(&tid[t], NULL, escritores, (void *)&id[t])) { printf("--ERRO: pthread_create()\n"); exit(-1); }
	}
	for (t=L; t<NTHREADS; t++) {
		id[t]=t-L;
		if (pthread_create(&tid[t], NULL, leitores, (void *)&id[t])) { printf("--ERRO: pthread_create()\n"); exit(-1); }
	}  
	//--espera todas as threads terminarem
	for (t=0; t<NTHREADS; t++) {
		if (pthread_join(tid[t], NULL)) {
			printf("--ERRO: pthread_join() \n"); exit(-1); 
		} 
	} 
	pthread_exit(NULL);

}
