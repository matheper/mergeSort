#include <string.h>
#include <stdio.h>
#include <time.h>
#include <omp.h>

int *geraVetor(int tamanho){
    srand(time(NULL));
    int *vetor, i;
    vetor = (int *) malloc(sizeof(int)*tamanho);
    #pragma omp parallel for
    for(i=0;i<tamanho;i++)
        vetor[i]=rand()%100000000+1;
    return vetor;
}

void merge(int vec[], int vecSize, int tmp[]) {
    int mid;
    int i, j, k;

    mid = vecSize / 2;

    i = 0;
    j = mid;
    k = 0;
    while (i < mid && j < vecSize) {
        if (vec[i] < vec[j]) {
            tmp[k] = vec[i];
            ++i;
        }
        else {
            tmp[k] = vec[j];
            ++j;
        }
        ++k;
    }

    if (i == mid) {
        while (j < vecSize) {
            tmp[k] = vec[j];
            ++j;
            ++k;
        }
    }
    else {
        while (i < mid) {
            tmp[k] = vec[i];
            ++i;
            ++k;
        }
    }

    for (i = 0; i < vecSize; ++i) {
        vec[i] = tmp[i];
    }

}

void mergesort_serial(int a[], int size, int temp[]) {
    if (size > 1){
        mergesort_serial(a, size/2, temp);
        mergesort_serial(a + size/2, size - size/2, temp);
        merge(a, size, temp);
    }
}

void mergesort_parallel_omp
(int a[], int size, int temp[], int threads) {
    if ( threads == 1) { mergesort_serial(a, size, temp); }
    else if ( threads == 4){
        #pragma omp parallel sections
        {
            #pragma omp section
            mergesort_serial(a, size/4, temp);
            #pragma omp section
            mergesort_serial(a + size/4, size/4, temp + size/4);
            #pragma omp section
            mergesort_serial(a + size/4*2, size/4, temp + size/4*2);
            #pragma omp section
            mergesort_serial(a + size/4*3, size - size/4*3, temp + size/4*3);
        }
        #pragma omp parallel sections
        {
            #pragma omp section
            merge(a,size/2,temp);
            #pragma omp section
            merge(a+size/2,size - size/2,temp+size/2);
        }
        merge(a,size,temp);
    }
    else if (threads > 1) {
        #pragma omp parallel sections
        {
            #pragma omp section
            mergesort_parallel_omp(a, size/2, temp, threads/2);
            #pragma omp section
            mergesort_parallel_omp(a + size/2, size - size/2,
                    temp + size/2, threads - threads/2);
        }
        merge(a, size, temp);
    } // threads > 1
}

int main(){
    int *vetor, *tmp, *base, tamanho, i, id, threads, nThreads, verbose;
    printf("Exibir vetor na tela?\n1:Sim\n0:Nao\n");
    scanf("%d",&verbose);
    printf("Digite tamanho do vetor: ");
    scanf("%d", &tamanho);
    while(tamanho > 0){
        printf("Digite numero de threads: ");
        scanf("%d",&nThreads);
        base = geraVetor(tamanho);
        while(nThreads > 0){
            omp_set_num_threads(nThreads);
            tmp = (int *)malloc(sizeof(int)*tamanho);
            vetor = (int *)malloc(sizeof(int)*tamanho);
            memcpy(vetor,base,sizeof(int)*tamanho);
            if(verbose>0)
                for(i=0;i<tamanho;i++)
                    printf("%d\n", vetor[i]);
//            clock_t start, end;
            time_t ini, fim;
//            start = clock();
            ini = time(NULL);
            mergesort_parallel_omp(vetor, tamanho, tmp, nThreads);
            fim = time(NULL);
//            end = clock();
            if(verbose>0){
                printf("\n\n");
                for(i=0;i<tamanho;i++)
                    printf("%d\n", vetor[i]);
            }
            printf("\n\nTempo gasto: %f\n",difftime(fim,ini));
//            printf("\n\nTempo gasto: %4.0fms\n",1000*(double)(end-start)/(double)(CLOCKS_PER_SEC));
            printf("Digite numero de threads: ");
            scanf("%d",&nThreads);
            free(tmp);
            free(vetor);
        }
        printf("Digite tamanho do vetor: ");
        scanf("%d", &tamanho);
        free(base);
    }
    return 0;
}
