#include<ptrtimer.h>
#include<stdlib.h>
#include<stdio.h>
#include<config.h>
#include<pthread.h>
#include <sys/sysinfo.h>

#include <unistd.h>

/* How big are we doing for the tests*/
#define TOTALSIZE 12000000000
// 1k=10 2k=11 4k=12 8k=13 16k=14 32k=15 64k=16 128k=17 256k=18 512k=19 
// 1M=20 2M=21 4M=22 8M=23 16M=24 32M=25 64M=26 128M=27 256M=28 512M=29 
// 1G=30 ...
#define LOOPMIN 12 
#define LOOPMAX 33



/* Here we have thread management */
#define THREADMIN 1
#define THREADMAX 8

pthread_barrier_t   barrier1;
pthread_barrier_t   barrier2;

int                 waitvar = 0;
pthread_cond_t      cond  = PTHREAD_COND_INITIALIZER;
pthread_mutex_t     mutex = PTHREAD_MUTEX_INITIALIZER;

struct thread_args {
        float *x;
        float *a;
        float *b;
        unsigned long start;
        unsigned long stop;
};

/* Math kernels*/


/*  x[k] *= n  */
void *out1in0constant_simple(void *function_args)
{
    float            *x = ((struct thread_args*)function_args)->x;
    unsigned long start = ((struct thread_args*)function_args)->start;
    unsigned long stop  = ((struct thread_args*)function_args)->stop;
    for(; start < stop; start++) {
                x[start] = 1.0;          
        
    }
}

void *out1in0cast1_simple(void *function_args)
{
    float            *x = ((struct thread_args*)function_args)->x;
    unsigned long start = ((struct thread_args*)function_args)->start;
    unsigned long stop  = ((struct thread_args*)function_args)->stop;
    for(; start < stop; start++) {
                x[start] = (float)start;          
        
    }
}

void *out1in0cast1mul1_simple(void *function_args)
{
    float            *x = ((struct thread_args*)function_args)->x;
    unsigned long start = ((struct thread_args*)function_args)->start;
    unsigned long stop  = ((struct thread_args*)function_args)->stop;
    for(; start < stop; start++) {
                x[start] = (float)start*1.5;          
        
    }
}

void *out1in0cast1mul1add1_simple(void *function_args)
{
    float            *x = ((struct thread_args*)function_args)->x;
    unsigned long start = ((struct thread_args*)function_args)->start;
    unsigned long stop  = ((struct thread_args*)function_args)->stop;
    for(; start < stop; start++) {
                x[start] = (float)start*1.5+0.3;          
        
    }
}

void *out1in0mul1add1_simple(void *function_args)
{
    float            *x = ((struct thread_args*)function_args)->x;
    unsigned long start = ((struct thread_args*)function_args)->start;
    unsigned long stop  = ((struct thread_args*)function_args)->stop;
    float  i = -0.0002;
    for(; start < stop; start++) {
                x[start] = i*1.5+0.3;
                i = i + 0.0001;
        
    }
}

/*  x[k] *= n  */
void *out1in1mul1_simple(void *function_args)
{
    float            *x = ((struct thread_args*)function_args)->x;
    unsigned long start = ((struct thread_args*)function_args)->start;
    unsigned long stop  = ((struct thread_args*)function_args)->stop;
    for(; start < stop; start++) {
                x[start] *= 1.07;          
        
    }
}

/*  x[k] *= n  */
void *out1in1mul1_cond(void *function_args)
{
    float            *x = ((struct thread_args*)function_args)->x;
    unsigned long start = ((struct thread_args*)function_args)->start;
    unsigned long stop  = ((struct thread_args*)function_args)->stop;
//     printf("Thread lock\n");
    pthread_mutex_lock(&mutex);
//     printf("Thread Wait\n");
    while(waitvar == 0)
        pthread_cond_wait(&cond,&mutex);
//     printf("Thread unlock\n");
    pthread_mutex_unlock(&mutex);
//     printf("Thread Going\n");
    for(; start < stop; start++) {
                x[start] *= 1.07;          
        }
}

/*  x[k] *= n  */
void *out1in1mul1(void *function_args)
{
    float            *x = ((struct thread_args*)function_args)->x;
    unsigned long start = ((struct thread_args*)function_args)->start;
    unsigned long stop  = ((struct thread_args*)function_args)->stop;
    pthread_barrier_wait (&barrier1);  
    for(; start < stop; start++) {
                x[start] *= 1.07;          
        }
    pthread_barrier_wait (&barrier2);
}


/*  x[k] += n  */
void *out1in1add1(void *function_args)
{
    float            *x = ((struct thread_args*)function_args)->x;
    unsigned long start = ((struct thread_args*)function_args)->start;
    unsigned long stop  = ((struct thread_args*)function_args)->stop;
    pthread_barrier_wait (&barrier1);
    for(; start < stop; start++) {
                x[start] += 1.07;          
        }
    pthread_barrier_wait (&barrier2);
}

/*  x[k] = x[k]*n*b  */
void *out1in1mul1add1(void *function_args)
{
    float            *x = ((struct thread_args*)function_args)->x;
    unsigned long start = ((struct thread_args*)function_args)->start;
    unsigned long stop  = ((struct thread_args*)function_args)->stop;
    pthread_barrier_wait (&barrier1);
    for(; start < stop; start++) {
                x[start] = x[start]*1.07+1.2;          
        }
    pthread_barrier_wait (&barrier2);
}

/*  x[k] = a[k]*b[k]  */
void *out1in2mul1(void *function_args)
{
    float            *x = ((struct thread_args*)function_args)->x;
    float            *a = ((struct thread_args*)function_args)->a;
    float            *b = ((struct thread_args*)function_args)->b;
    unsigned long start = ((struct thread_args*)function_args)->start;
    unsigned long stop  = ((struct thread_args*)function_args)->stop;
    pthread_barrier_wait (&barrier1);
    for(; start < stop; start++) {
                x[start] = a[start]*b[start];          
        }
    pthread_barrier_wait (&barrier2);
}

/*  x[k] = a[k]*b[k]  */
void *out1in3mul1add1(void *function_args)
{
    float            *x = ((struct thread_args*)function_args)->x;
    float            *a = ((struct thread_args*)function_args)->a;
    float            *b = ((struct thread_args*)function_args)->b;
    unsigned long start = ((struct thread_args*)function_args)->start;
    unsigned long stop  = ((struct thread_args*)function_args)->stop;
    pthread_barrier_wait (&barrier1);
    for(; start < stop; start++) {
                x[start] = a[start]*b[start]+x[start];          
        }
    pthread_barrier_wait (&barrier2);
}

/* And where we start everything */
int main(void)
{
        // Generic variables
        float *x, *a, *b;
        unsigned long i,j,k;
        unsigned long rep;
        unsigned long loopmin = 1UL<<LOOPMIN;
        unsigned long loopmax = 1UL<<LOOPMAX;
        unsigned long loopsize;
        
        /*Multi threading stuff here*/
        unsigned long Nthreads = 16;
        struct thread_args **function_args = (struct thread_args **)malloc(sizeof(struct thread_args*)*Nthreads);
        for(k = 0; k < Nthreads; k++)
            function_args[k] = (struct thread_args *)malloc(sizeof(struct thread_args));
        pthread_t *tid = (pthread_t*)malloc(sizeof(pthread_t)*Nthreads);
        
        ptrtimer *t0;
        t0 = ptrtimer_init(10);

        printf("Compiler " ptrbench_C_COMPILER_ID "\n Version " ptrbench_C_COMPILER_VERSION "\n CFLAGS " ptrbench_CFLAGS "\n");

        // Adjust the loop parameters
        loopsize = (TOTALSIZE / sizeof(float)) / 3;
        printf("Total memory allocated %ld Mbytes \n", loopsize * 3 * sizeof(float) / (1024 * 1024));
        printf("Loop size              %ld Mfloats\n", loopsize / (1024 * 1024));

        if (loopmin > loopsize)
                loopmin = loopsize;
        if (loopmax > loopsize)
                loopmax = loopsize;

        printf("Loop min size          %ld floats\n", loopmin);
        printf("Loop max size          %ld floats\n", loopmax);

        // Allocate arrays
        x = malloc(loopsize * sizeof(float));
        a = malloc(loopsize * sizeof(float));
        b = malloc(loopsize * sizeof(float));

        printf("a[] = (float)k single thread\n");
        for (i = loopmin; i<loopsize; i = i<<1) {
                ptrtimer_reset(t0);
                rep = loopsize /i;
                if (loopsize / i > 10000)
                    rep = 10000;
                for (j = 0; j < rep; j++) {
                        (function_args[0])->x = x;
                        (function_args[0])->start = 0;
                        (function_args[0])->stop  = i;
                        ptrtimer_start(t0);
                        out1in0cast1_simple((void *)(function_args[0]));
                        ptrtimer_stop(t0);                     
                }
                printf("size=%ld rep=%ld Mcast/s=%4.3f MByte/s=%4.3f \n",i, rep,
                        (double)i / ptrtimer_getavg(t0) * 1.0 / 1000000.0,
                        (double)i / ptrtimer_getavg(t0) * 1 * sizeof(float) / 1000000.0);
        }
        
        printf("a[] = (float)k*m single thread\n");
        for (i = loopmin; i<loopsize; i = i<<1) {
                ptrtimer_reset(t0);
                rep = loopsize /i;
                if (loopsize / i > 10000)
                    rep = 10000;
                for (j = 0; j < rep; j++) {
                        (function_args[0])->x = a;
                        (function_args[0])->start = 0;
                        (function_args[0])->stop  = i;
                        ptrtimer_start(t0);
                        out1in0cast1mul1_simple((void *)(function_args[0]));
                        ptrtimer_stop(t0);                     
                }
                printf("size=%ld rep=%ld Mflop/s=%4.3f MByte/s=%4.3f \n",i, rep,
                        (double)i / ptrtimer_getavg(t0) * 1.0 / 1000000.0,
                        (double)i / ptrtimer_getavg(t0) * 1 * sizeof(float) / 1000000.0);
        }        

        printf("a[] = (float)k*m+b single thread\n");
        for (i = loopmin; i<loopsize; i = i<<1) {
                ptrtimer_reset(t0);
                rep = loopsize /i;
                if (loopsize / i > 10000)
                    rep = 10000;
                for (j = 0; j < rep; j++) {
                        (function_args[0])->x = b;
                        (function_args[0])->start = 0;
                        (function_args[0])->stop  = i;
                        ptrtimer_start(t0);
                        out1in0cast1mul1add1_simple((void *)(function_args[0]));
                        ptrtimer_stop(t0);                     
                }
                printf("size=%ld rep=%ld Mflop/s=%4.3f MByte/s=%4.3f \n",i, rep,
                        (double)i / ptrtimer_getavg(t0) * 2.0 / 1000000.0,
                        (double)i / ptrtimer_getavg(t0) * 1 * sizeof(float) / 1000000.0);
        }   
        
        printf("a[] =(float)k*m+b simple fork\n");
        for (i = loopmin; i<loopsize; i = i<<1) {
                ptrtimer_reset(t0);
                rep = loopsize /i;
                if (loopsize / i > 10000)
                    rep = 10000;
                for (j = 0; j < rep; j++) {
                        for(k = 0; k < Nthreads; k++) {
                                (function_args[k])->x = x;
                                (function_args[k])->start = (i/Nthreads)*k;
                                (function_args[k])->stop  = (i/Nthreads)*(k+1);
                        }
                        ptrtimer_start(t0);
                        for(k = 0; k < Nthreads; k++)
                                pthread_create(&(tid[k]),NULL,out1in0cast1mul1add1_simple,(void *)(function_args[k]));
                        for(k = 0; k < Nthreads; k++)
                                pthread_join(tid[k],NULL);
                        ptrtimer_stop(t0);
                }
                printf("size=%ld rep=%ld Mflop/s=%4.3f MByte/s=%4.3f \n",i, rep,
                        (double)i / ptrtimer_getavg(t0) * 2.0 / 1000000.0,
                        (double)i / ptrtimer_getavg(t0) * 1 * sizeof(float) / 1000000.0);
        }
        
        printf("a[] =k*m+b simple fork\n");
        for (i = loopmin; i<loopsize; i = i<<1) {
                ptrtimer_reset(t0);
                rep = loopsize /i;
                if (loopsize / i > 10000)
                    rep = 10000;
                for (j = 0; j < rep; j++) {
                        for(k = 0; k < Nthreads; k++) {
                                (function_args[k])->x = a;
                                (function_args[k])->start = (i/Nthreads)*k;
                                (function_args[k])->stop  = (i/Nthreads)*(k+1);
                        }
                        ptrtimer_start(t0);
                        for(k = 0; k < Nthreads; k++)
                                pthread_create(&(tid[k]),NULL,out1in0mul1add1_simple,(void *)(function_args[k]));
                        for(k = 0; k < Nthreads; k++)
                                pthread_join(tid[k],NULL);
                        ptrtimer_stop(t0);
                }
                printf("size=%ld rep=%ld Mflop/s=%4.3f MByte/s=%4.3f \n",i, rep,
                        (double)i / ptrtimer_getavg(t0) * 2.0 / 1000000.0,
                        (double)i / ptrtimer_getavg(t0) * 1 * sizeof(float) / 1000000.0);
        }
        
        printf("a[] *= m single thread\n");
        for (i = loopmin; i<loopsize; i = i<<1) {
                ptrtimer_reset(t0);
                rep = loopsize /i;
                if (loopsize / i > 10000)
                    rep = 10000;
                for (j = 0; j < rep; j++) {
                        (function_args[0])->x = x;
                        (function_args[0])->start = 0;
                        (function_args[0])->stop  = i;
                        ptrtimer_start(t0);
                        out1in1mul1_simple((void *)(function_args[0]));
                        ptrtimer_stop(t0);                     
                }
                printf("size=%ld rep=%ld Mflop/s=%4.3f MByte/s=%4.3f \n",i, rep,
                        (double)i / ptrtimer_getavg(t0) * 1.0 / 1000000.0,
                        (double)i / ptrtimer_getavg(t0) * 2 * sizeof(float) / 1000000.0);
        }

        printf("a[] *= m simple fork\n");
        for (i = loopmin; i<loopsize; i = i<<1) {
                ptrtimer_reset(t0);
                rep = loopsize /i;
                if (loopsize / i > 10000)
                    rep = 10000;
                for (j = 0; j < rep; j++) {
                        for(k = 0; k < Nthreads; k++) {
                                (function_args[k])->x = x;
                                (function_args[k])->start = (i/Nthreads)*k;
                                (function_args[k])->stop  = (i/Nthreads)*(k+1);
                        }
                        ptrtimer_start(t0);
                        for(k = 0; k < Nthreads; k++)
                                pthread_create(&(tid[k]),NULL,out1in1mul1_simple,(void *)(function_args[k]));
                        for(k = 0; k < Nthreads; k++)
                                pthread_join(tid[k],NULL);
                        ptrtimer_stop(t0);
                }
                printf("size=%ld rep=%ld Mflop/s=%4.3f MByte/s=%4.3f \n",i, rep,
                        (double)i / ptrtimer_getavg(t0) * 1.0 / 1000000.0,
                        (double)i / ptrtimer_getavg(t0) * 2 * sizeof(float) / 1000000.0);
        }

        printf("a[] *= m cond wait\n");
        for (i = loopmin; i<loopsize; i = i<<1) {
                ptrtimer_reset(t0);
                rep = loopsize /i;
                if (loopsize / i > 10000)
                    rep = 10000;
                for (j = 0; j < rep; j++) {
                        pthread_mutex_init(&mutex, NULL);
                        pthread_cond_init(&cond, NULL);
                        waitvar = 0;
                        for(k = 0; k < Nthreads; k++) {
                                (function_args[k])->x = x;
                                (function_args[k])->start = (i/Nthreads)*k;
                                (function_args[k])->stop  = (i/Nthreads)*(k+1);
                        }
//                         printf("Thread create\n");
//                         printf("Main lock\n");
                        pthread_mutex_lock(&mutex);
                        for(k = 0; k < Nthreads; k++)
                                pthread_create(&(tid[k]),NULL,out1in1mul1_cond,(void *)(function_args[k]));
                        
                        waitvar = 1;                        
                        //usleep(10000);
                        ptrtimer_start(t0);
//                         printf("Main Broadcast\n");
                        pthread_cond_broadcast(&cond);
//                         printf("Main unlock\n");
                        pthread_mutex_unlock(&mutex); 
//                         printf("Main join\n");
                        for(k = 0; k < Nthreads; k++)
                                pthread_join(tid[k],NULL);
                        ptrtimer_stop(t0);                        
                        pthread_cond_destroy(&cond);
                        pthread_mutex_destroy(&mutex);
                }
                printf("size=%ld rep=%ld Mflop/s=%4.3f MByte/s=%4.3f \n",i, rep,
                        (double)i / ptrtimer_getavg(t0) * 1.0 / 1000000.0,
                        (double)i / ptrtimer_getavg(t0) * 2 * sizeof(float) / 1000000.0);
        }

        printf("a[] *= m barrier\n");
        for (i = loopmin; i<loopsize; i = i<<1) {
                ptrtimer_reset(t0);
                rep = loopsize /i;
                if (loopsize / i > 10000)
                    rep = 10000;
                for (j = 0; j < rep; j++) {
                        pthread_barrier_init (&barrier1, NULL, Nthreads+1);
                        pthread_barrier_init (&barrier2, NULL, Nthreads+1);
                        for(k = 0; k < Nthreads; k++) {
                                (function_args[k])->x = x;
                                (function_args[k])->start = (i/Nthreads)*k;
                                (function_args[k])->stop  = (i/Nthreads)*(k+1);
                        }
                        for(k = 0; k < Nthreads; k++)
                                pthread_create(&(tid[k]),NULL,out1in1mul1,(void *)(function_args[k]));
                        ptrtimer_start(t0);
                        pthread_barrier_wait (&barrier1);
                        pthread_barrier_wait (&barrier2);
                        ptrtimer_stop(t0);                        
                        for(k = 0; k < Nthreads; k++)
                                pthread_join(tid[k],NULL);
                        pthread_barrier_destroy(&barrier1);
                        pthread_barrier_destroy(&barrier2);                    
                }
                printf("size=%ld rep=%ld Mflop/s=%4.3f MByte/s=%4.3f \n",i, rep,
                        (double)i / ptrtimer_getavg(t0) * 1.0 / 1000000.0,
                        (double)i / ptrtimer_getavg(t0) * 2 * sizeof(float) / 1000000.0);
        }


        printf("a[] += m\n");
        for (i = loopmin; i<loopsize; i = i<<1) {
                ptrtimer_reset(t0);
                rep = loopsize /i;
                if (loopsize / i > 10000)
                    rep = 10000;
                for (j = 0; j < rep; j++) {
                        pthread_barrier_init (&barrier1, NULL, Nthreads+1);
                        pthread_barrier_init (&barrier2, NULL, Nthreads+1);
                        for(k = 0; k < Nthreads; k++) {
                                (function_args[k])->x = x;
                                (function_args[k])->start = (i/Nthreads)*k;
                                (function_args[k])->stop  = (i/Nthreads)*(k+1);
                        }
                        for(k = 0; k < Nthreads; k++)
                                pthread_create(&(tid[k]),NULL,out1in1add1,(void *)(function_args[k]));
                        ptrtimer_start(t0);
                        pthread_barrier_wait (&barrier1);
                        pthread_barrier_wait (&barrier2);
                        ptrtimer_stop(t0);
                        for(k = 0; k < Nthreads; k++)
                                pthread_join(tid[k],NULL);
                        pthread_barrier_destroy(&barrier1);
                        pthread_barrier_destroy(&barrier2);                    
                }
                printf("size=%ld rep=%ld Mflop/s=%4.3f MByte/s=%4.3f \n",i, rep,
                        (double)i / ptrtimer_getavg(t0) * 1.0 / 1000000.0,
                        (double)i / ptrtimer_getavg(t0) * 2 * sizeof(float) / 1000000.0);
        }

        printf("a[] = a[]*m+b\n");
        for (i = loopmin; i<loopsize; i = i<<1) {
                ptrtimer_reset(t0);
                rep = loopsize /i;
                if (loopsize / i > 10000)
                    rep = 10000;
                for (j = 0; j < rep; j++) {
                        pthread_barrier_init (&barrier1, NULL, Nthreads+1);
                        pthread_barrier_init (&barrier2, NULL, Nthreads+1);
                        for(k = 0; k < Nthreads; k++) {
                                (function_args[k])->x = x;
                                (function_args[k])->start = (i/Nthreads)*k;
                                (function_args[k])->stop  = (i/Nthreads)*(k+1);
                        }
                        for(k = 0; k < Nthreads; k++)
                                pthread_create(&(tid[k]),NULL,out1in1mul1add1,(void *)(function_args[k]));
                        ptrtimer_start(t0);
                        pthread_barrier_wait (&barrier1);
                        pthread_barrier_wait (&barrier2);
                        ptrtimer_stop(t0);
                        for(k = 0; k < Nthreads; k++)
                                pthread_join(tid[k],NULL);
                        pthread_barrier_destroy(&barrier1);
                        pthread_barrier_destroy(&barrier2);                    
                
                }
                printf("size=%ld rep=%ld Mflop/s=%4.3f MByte/s=%4.3f \n",i, rep,
                        (double)i / ptrtimer_getavg(t0) * 2.0 / 1000000.0,
                        (double)i / ptrtimer_getavg(t0) * 2 * sizeof(float) / 1000000.0);
        }
        printf("x[] = a[]*b[]\n");
        for (i = loopmin; i<loopsize; i = i<<1) {
                ptrtimer_reset(t0);
                rep = loopsize /i;
                if (loopsize / i > 10000)
                    rep = 10000;
                for (j = 0; j < rep; j++) {
                        pthread_barrier_init (&barrier1, NULL, Nthreads+1);
                        pthread_barrier_init (&barrier2, NULL, Nthreads+1);
                        for(k = 0; k < Nthreads; k++) {
                                (function_args[k])->a = a;
                                (function_args[k])->b = b;
                                (function_args[k])->x = x;
                                (function_args[k])->start = (i/Nthreads)*k;
                                (function_args[k])->stop  = (i/Nthreads)*(k+1);
                        }
                        for(k = 0; k < Nthreads; k++)
                                pthread_create(&(tid[k]),NULL,out1in2mul1,(void *)(function_args[k]));
                        ptrtimer_start(t0);
                        pthread_barrier_wait (&barrier1);
                        pthread_barrier_wait (&barrier2);
                        ptrtimer_stop(t0);
                        for(k = 0; k < Nthreads; k++)
                                pthread_join(tid[k],NULL);
                        pthread_barrier_destroy(&barrier1);
                        pthread_barrier_destroy(&barrier2);                    
                
                }
                printf("size=%ld rep=%ld Mflop/s=%4.3f MByte/s=%4.3f \n",i, rep,
                        (double)i / ptrtimer_getavg(t0) * 1.0 / 1000000.0,
                        (double)i / ptrtimer_getavg(t0) * 3 * sizeof(float) / 1000000.0);
        }

        printf("x[] = a[]*b[]+x[]\n");
        for (i = loopmin; i<loopsize; i = i<<1) {
                ptrtimer_reset(t0);
                rep = loopsize /i;
                if (loopsize / i > 10000)
                    rep = 10000;
                for (j = 0; j < rep; j++) {
                        pthread_barrier_init (&barrier1, NULL, Nthreads+1);
                        pthread_barrier_init (&barrier2, NULL, Nthreads+1);
                        for(k = 0; k < Nthreads; k++) {
                                (function_args[k])->a = a;
                                (function_args[k])->b = b;
                                (function_args[k])->x = x;
                                (function_args[k])->start = (i/Nthreads)*k;
                                (function_args[k])->stop  = (i/Nthreads)*(k+1);
                        }
                        for(k = 0; k < Nthreads; k++)
                                pthread_create(&(tid[k]),NULL,out1in3mul1add1,(void *)(function_args[k]));
                        ptrtimer_start(t0);
                        pthread_barrier_wait (&barrier1);
                        pthread_barrier_wait (&barrier2);
                        ptrtimer_stop(t0);
                        for(k = 0; k < Nthreads; k++)
                                pthread_join(tid[k],NULL);
                        pthread_barrier_destroy(&barrier1);
                        pthread_barrier_destroy(&barrier2);                    
                
                }
                printf("size=%ld rep=%ld Mflop/s=%4.3f MByte/s=%4.3f \n",i, rep,
                        (double)i / ptrtimer_getavg(t0) * 2.0 / 1000000.0,
                        (double)i / ptrtimer_getavg(t0) * 4.0 * sizeof(float) / 1000000.0);
        }
        
        
        
        // Clean up
        ptrtimer_close(t0);
        // Thread clean up
        for(k = 0; k < Nthreads; k++)
            free(function_args[k]);
        free(function_args);
        //pthread_mutex_destroy();
        
        // Testing variables clean up
        free(b);
        free(a);
        free(x);

        return 0;
}
