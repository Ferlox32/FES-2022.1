#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include "timer.h"

long long int dim;
int nthreads; //numero de threads
int *vetor; //vetor de entrada com dimensao dim
long long int s = 0;
pthread_mutex_t mutex;

int ehPrimo(int n){
    double root = sqrt(n);
    if(n <= 2){
        return 0;
    }
    for(int i = 2; i <= root; i++){
        if (!(n % i)){
            return 0;
        }
    }
    return 1;
}

//fluxo das threads
void *processaPrimos(void* arg){
    double* vetorLocal = (double *) arg;
    int i = 0;
    while(i < dim){
        //garante que s so aumentara em uma thread por vez
        pthread_mutex_lock(&mutex);
        i = s++;
        pthread_mutex_unlock(&mutex);
        if (ehPrimo(vetor[i]))
            vetorLocal[i] = sqrt(vetor[i]);
        else
            vetorLocal[i] = vetor[i];
    }
    pthread_exit(NULL);
}

//fluxo principal
int main(int argc, char *argv[])
{
    double ini, fim; //tomada de tempo
    pthread_t *tid; //identificadores das threads no sistema
    float * vetorS, * vetorC; //valor de retorno das threads
    double tempoS, tempoC;

    //recebe e valida os parametros de entrada (dimensao do vetor, numero de threads)
    if (argc < 3){
        fprintf(stderr, "Digite: %s <dimensao do vetor> <numero threads>\n", argv[0]);
        return 1;
    }
    dim = atoll(argv[1]);
    nthreads = atoi(argv[2]);
    //aloca o vetor de entrada
    vetor = (int *)malloc(sizeof(int) * dim);
    if (vetor == NULL){
        fprintf(stderr, "ERRO--malloc\n");
        return 2;
    }
    vetorS = (float *)malloc(sizeof(float) *  dim);
    vetorC = (float *)malloc(sizeof(float) *  dim);
    //preenche o vetor de entrada
    srand(time(NULL));
    for (long int i = 0; i < dim; i++)
        vetor[i] = rand() % dim;

    //soma sequencial dos elementos
    GET_TIME(ini);
    for (long long int i = 0; i < dim; i++){
        if (ehPrimo(vetor[i]))
            vetorS[i] = sqrt(vetor[i]);
        else
            vetorS[i] = vetor[i];
    }
    GET_TIME(fim);
    tempoS = fim - ini;
    printf("Tempo sequencial:  %lf\n", tempoS);

    //soma concorrente dos elementos
    GET_TIME(ini);
    tid = (pthread_t *)malloc(sizeof(pthread_t) * nthreads);
    if (tid == NULL)
    {
        fprintf(stderr, "ERRO--malloc\n");
        return 2;
    }
    //criar as threads
    pthread_mutex_init(&mutex, NULL);
    for (long int i = 0; i < nthreads; i++){
        if (pthread_create(tid + i, NULL, processaPrimos, (void *)vetorC)){
            fprintf(stderr, "ERRO--pthread_create\n");
            return 3;
        }
    }
    //aguardar o termino das threads
    for (long int i = 0; i < nthreads; i++){
        if (pthread_join(*(tid + i), NULL)){
            fprintf(stderr, "ERRO--pthread_create\n");
            return 3;
        }
    }
    pthread_mutex_destroy(&mutex);
    GET_TIME(fim);
    tempoC = fim - ini;
    printf("Tempo concorrente:  %lf\n", tempoC);

    //exibir os resultados
    printf("Tempo Sequencial / Tempo Concorrente: %lf\n", tempoS/tempoC);

    //libera as areas de memoria alocadas
    free(vetor);
    free(tid);

    return 0;
}
