#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define NTHREADS 5
#define TCOUNT 20

int x = 0;
pthread_mutex_t lock;
pthread_cond_t cond;
const char *ordens[5] = {"Volte sempre!", "Fique a vontade.", "Sente-se por favor.", "Aceita um copo dagua?.", "Seja bem-vindo!"};

void *garcom(void *arg)
{
    long int id = (long int)arg;
    pthread_mutex_lock(&lock);
    if (x == 0 && id != 5){
        pthread_cond_wait(&cond, &lock);
    }
    pthread_mutex_unlock(&lock);
    pthread_mutex_lock(&lock);
    while (x < 4 && id == 1){
        pthread_cond_wait(&cond, &lock);
    }
    x++;
    pthread_mutex_unlock(&lock);

    printf("%s\n", ordens[id - 1]);
    pthread_cond_broadcast(&cond);

    pthread_exit(NULL);
}

int main()
{
    pthread_t *tid;
    tid = (pthread_t *)malloc(sizeof(pthread_t) * NTHREADS);
    pthread_mutex_init(&lock, NULL);
    pthread_cond_init(&cond, NULL);
    for (long i = 1; i <= NTHREADS; i++){
        if (pthread_create(tid+i, NULL, garcom, (void *)i)){
            fprintf(stderr, "ERRO--pthread_create\n");
            return 3;
        };
    }
    for (int i = 0; i < NTHREADS; i++){
        pthread_join(*(tid + i), NULL);
    }
    pthread_mutex_destroy(&lock);
    pthread_cond_destroy(&cond);
	
    return 0;
}
