#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "timer.h"

long long int dim; //dimensao do vetor de entrada
int nthreads; //numero de threads
double *vetor; //vetor de entrada com dimensao dim

typedef struct {
    double maior, menor;
} MaximoMinimo;

//fluxo das threads
void *tarefa(void *arg)
{
    long int id = (long int)arg; //identificador da thread
    MaximoMinimo *obj;
    obj = (MaximoMinimo *)malloc(sizeof(MaximoMinimo));
    if (obj == NULL){
        exit(1);
    }
    //soma os elementos do bloco da thread
    obj ->maior = vetor[id];
    obj -> menor = vetor[id];
    
    for (long int i = id; i < dim; i+= nthreads){
        obj -> maior = vetor[i] > obj -> maior ? vetor[i] : obj -> maior;
        obj -> menor = vetor[i] < obj -> menor ? vetor[i] : obj -> menor;
    }
    //retorna o resultado da soma local
    pthread_exit((void *)obj);
}

//fluxo principal
int main(int argc, char *argv[])
{
    double ini, fim; //tomada de tempo
    pthread_t *tid; //identificadores das threads no sistema
    double *retorno; //valor de retorno das threads
    double tempoS, tempoC;
    double max1, max2, min1, min2;

    //recebe e valida os parametros de entrada (dimensao do vetor, numero de threads)
    if (argc < 3){
        fprintf(stderr, "Digite: %s <dimensao do vetor> <numero threads>\n", argv[0]);
        return 1;
    }
    dim = atoll(argv[1]);
    nthreads = atoi(argv[2]);
    //aloca o vetor de entrada
    vetor = (double *)malloc(sizeof(double) * dim/10 * 10);
    if (vetor == NULL){
        fprintf(stderr, "foi aqui\n");
        fprintf(stderr, "ERRO--malloc\n");
        return 2;
    }
    //preenche o vetor de entrada
    srand(time(NULL));
    for (long int i=0; i<dim; i++)
        vetor[i] = rand() % dim;

    //soma sequencial dos elementos
    GET_TIME(ini);
    max1 = vetor[0];
    min1 = vetor[0];
    for (long long int i = 0; i < dim; i++){
        max1 = vetor[i] > max1 ? vetor[i] : max1;
        min1 = vetor[i] < min1 ? vetor[i] : min1;
    }
    GET_TIME(fim);
    tempoS = fim - ini;
    printf("Tempo sequencial:  %lf\n", tempoS);

    //soma concorrente dos elementos
    GET_TIME(ini);
    tid = (pthread_t *)malloc(sizeof(pthread_t) * nthreads);
    if (tid == NULL){
        fprintf(stderr, "foi aqui 2\n");
        fprintf(stderr, "ERRO--malloc\n");
        return 2;
    }
    max2 = vetor[0];
    min2 = vetor[0];
    // criar as threads
    for (long int i=0; i<nthreads; i++){
        if (pthread_create(tid + i, NULL, tarefa, (void *)i)){
            fprintf(stderr, "ERRO--pthread_create\n");
            return 3;
        }
    }
    // aguardar o termino das threads
    for (long int i = 0; i < nthreads; i++){
        if (pthread_join(*(tid + i), (void **)&retorno)){
            fprintf(stderr, "ERRO--pthread_create\n");
            return 3;
        }
        // soma global
        MaximoMinimo *obj = (MaximoMinimo*) retorno;
        max2 = obj -> maior > max2 ? obj -> maior : max2;
        min2 = obj -> menor < min2 ? obj -> menor : min2;
    }
    GET_TIME(fim);
    tempoC = fim - ini;
    printf("Tempo concorrente:  %lf\n", tempoC);

    // exibir os resultadoss
    printf("Maior elemento Sequencial:  %.12lf Menor Elemento concorrente: %.12lf\n", max1, min1);
    printf("Maior elemento conc: %.12lf Menor elemento concorrente: %.12lf\n", max2, min2);
    printf("Tempo Sequencial / Tempo Concorrente: %lf\n", tempoS/tempoC);

    // libera as areas de memoria alocadas
    free(vetor);
    free(tid);

    return 0;
}