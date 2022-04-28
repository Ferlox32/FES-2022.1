/* Multiplicacao de matriz-vetor (considerando matrizes quadradas) */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "timer.h"

float *mat1; //matriz de entrada 1
float *mat2; //matriz de entrada 2
float *saidaConc; //matriz de saida concorrente
float *saidaSeq; //matriz de saida sequencial
int nthreads; //numero de threads

typedef struct
{
    int id;  // identificador do elemento que a thread ira processar
    int dim; // dimensao das estruturas de entrada
} tArgs;

// funcao que as threads executarao
void *tarefa(void *arg){
    tArgs *args = (tArgs *)arg;
    for (int i = args->id; i < args->dim; i += nthreads){
        for (int j = 0; j < args->dim; j++){
            for (int k = 0; k < args->dim; k++){
                saidaConc[i * (args->dim) + j] += mat1[i * (args->dim) + k] * mat2[k * (args->dim) + j];
            }
        }
    }
    pthread_exit(NULL);
}

// fluxo principal
int main(int argc, char *argv[])
{
    int dim;        // dimensao da matriz de entrada
    pthread_t *tid; // identificadores das threads no sistema
    tArgs *args;    // identificadores locais das threads e dimensao
    double inicio, fim, deltaConc, deltaSeq;

    // leitura e avaliacao dos parametros de entrada
    if (argc < 3){
        printf("Digite: %s <dimensao da matriz> <numero de threads>\n", argv[0]);
        return 1;
    }
    dim = atoi(argv[1]);
    nthreads = atoi(argv[2]);
    if (nthreads > dim) nthreads = dim;

    // alocacao de memoria para as estruturas de dados
    mat1 = (float *)malloc(sizeof(float) * dim * dim);
    if (mat1 == NULL){
        printf("ERRO--malloc\n");
        return 2;
    }
    mat2 = (float *)malloc(sizeof(float) * dim * dim);
    if (mat2 == NULL){
        printf("ERRO--malloc\n");
        return 2;
    }
    saidaConc = (float *)malloc(sizeof(float) * dim * dim);
    if (saidaConc == NULL){
        printf("ERRO--malloc\n");
        return 2;
    }
    saidaSeq = (float *)malloc(sizeof(float) * dim * dim);
    if (saidaSeq == NULL){
        printf("ERRO--malloc\n");
        return 2;
    }

    // inicializacao das estruturas de dados de entrada e saida
    srand(time(NULL));
    for (int i = 0; i < dim; i++){
        for (int j = 0; j < dim; j++){
            mat1[i * dim + j] = rand() % 10;
            mat2[i * dim + j] = rand() % 10;
        }
    }
    // alocacao das estruturas
    tid = (pthread_t *)malloc(sizeof(pthread_t) * nthreads);
    if (tid == NULL){
        puts("ERRO--malloc");
        return 2;
    }
    args = (tArgs *)malloc(sizeof(tArgs) * nthreads);
    if (args == NULL){
        puts("ERRO--malloc");
        return 2;
    }
    // criacao das threads
    GET_TIME(inicio);
    for (int i = 0; i < nthreads; i++){
        (args + i)->id = i;
        (args + i)->dim = dim;
        if (pthread_create(tid + i, NULL, tarefa, (void *)(args + i))){
            puts("ERRO--pthread_create");
            return 3;
        }
    }
    // espera pelo termino da threads
    for (int i=0; i<nthreads; i++) {
        pthread_join(*(tid+i), NULL);
    }
    GET_TIME(fim);
    deltaConc = fim - inicio;
    printf("Tempo multiplicacao conc: %lf\n", deltaConc);

    //multiplicacao sequencial
    GET_TIME(inicio);
    for (int i = 0; i < dim; i++){
        for (int j = 0; j < dim; j++){
            for (int k = 0; k < dim; k++){
                saidaSeq[i * (dim) + j] += mat1[i * (dim) + k] * mat2[k * (dim) + j];
            }
        }
    }
    GET_TIME(fim);
    deltaSeq = fim - inicio;
    printf("Tempo multiplicacao seq: %lf\n", deltaSeq);
    printf("Razão TSequencial/TConcorrente: %lf\n", deltaSeq/deltaConc);

    for (int i = 0; i < dim; i++){
        for (int j = 0; j < dim; j++){
            if(saidaConc[i * dim + j] != saidaSeq[i * dim + j]){
                printf("Incorreto\n");
                return 0;
            }
        }
    }
    printf("Correto\n");

    // liberacao da memoria
    free(mat1);
    free(mat2);
    free(saidaConc);
    free(saidaSeq);
    free(tid);
    free(args);

    return 0;
}
