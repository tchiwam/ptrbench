#include<ptrtimer.h>
#include<stdlib.h>
#include<stdio.h>
#include<config.h>
#include<pthread.h>
#include <sys/sysinfo.h>

#define TOTALSIZE 65536000000
// 1k=10 2k=11 4k=12 8k=13 16k=14 32k=15 64k=16 128k=17 256k=18 512k=19 
// 1M=20 2M=21 4M=22 8M=23 16M=24 32M=25 64M=26 128M=27 256M=28 512M=29 
// 1G=30 ...
#define LOOPMIN 16 
#define LOOPMAX 35
#define THREADMIN 1
#define THREADMAX 8


pthread_barrier_t   barrier1;
pthread_barrier_t   barrier2;


struct thread_args {
        float *x;
        unsigned long start;
        unsigned long stop;
};

/*  x[k] *= n  */
void *out1in1flop1mul(void *function_args)
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
void *out1in1flop1add(void *function_args)
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
void *out1in1flop1mul1add(void *function_args)
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

int main(void)
{
        // Generic variables
        float *x, *a, *b;
        unsigned long i,j,k;
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

        ptrtimer_start(t0);
        for(k = 0; k < loopsize; k++) {
                a[k] = (float)k / 0.5;
                b[k] = (float)k * 1.7;
                x[k] = (float)k * -0.6;
        }
        ptrtimer_stop(t0);
        ptrtimer_report(t0);
        
        printf("a[] *= m\n");
        for (i = loopmin; i<loopsize; i = i<<1) {
                ptrtimer_reset(t0);
                for (j = 0; j < loopsize / i; j++) {
                        pthread_barrier_init (&barrier1, NULL, Nthreads+1);
                        pthread_barrier_init (&barrier2, NULL, Nthreads+1);
                        for(k = 0; k < Nthreads; k++) {
                                (function_args[k])->x = x;
                                (function_args[k])->start = (i/Nthreads)*k;
                                (function_args[k])->stop  = (i/Nthreads)*(k+1);
                        }
                        for(k = 0; k < Nthreads; k++)
                                pthread_create(&(tid[k]),NULL,out1in1flop1mul,(void *)(function_args[k]));
                        ptrtimer_start(t0);
                        pthread_barrier_wait (&barrier1);
                        pthread_barrier_wait (&barrier2);
                        ptrtimer_stop(t0);                        
                        for(k = 0; k < Nthreads; k++)
                                pthread_join(tid[k],NULL);
                        pthread_barrier_destroy(&barrier1);
                        pthread_barrier_destroy(&barrier2);                    
                }
                printf("size=%ld rep=%ld Mflop/s=%4.3f MByte/s=%4.3f \n",i, loopsize/i,
                        (double)i / ptrtimer_getavg(t0) * 1.0 / 1000000.0,
                        (double)i / ptrtimer_getavg(t0) * 2 * sizeof(float) / 1000000.0);
        }


        printf("a[] += m\n");
        for (i = loopmin; i<loopsize; i = i<<1) {
                ptrtimer_reset(t0);
                for (j = 0; j < loopsize / i; j++) {
                        pthread_barrier_init (&barrier1, NULL, Nthreads+1);
                        pthread_barrier_init (&barrier2, NULL, Nthreads+1);
                        for(k = 0; k < Nthreads; k++) {
                                (function_args[k])->x = x;
                                (function_args[k])->start = (i/Nthreads)*k;
                                (function_args[k])->stop  = (i/Nthreads)*(k+1);
                        }
                        for(k = 0; k < Nthreads; k++)
                                pthread_create(&(tid[k]),NULL,out1in1flop1add,(void *)(function_args[k]));
                        ptrtimer_start(t0);
                        pthread_barrier_wait (&barrier1);
                        pthread_barrier_wait (&barrier2);
                        ptrtimer_stop(t0);
                        for(k = 0; k < Nthreads; k++)
                                pthread_join(tid[k],NULL);
                        pthread_barrier_destroy(&barrier1);
                        pthread_barrier_destroy(&barrier2);                    
                }
                printf("size=%ld rep=%ld Mflop/s=%4.3f MByte/s=%4.3f \n",i, loopsize/i,
                        (double)i / ptrtimer_getavg(t0) * 1.0 / 1000000.0,
                        (double)i / ptrtimer_getavg(t0) * 2 * sizeof(float) / 1000000.0);
        }

        printf("a[] = a[]*m+b\n");
        for (i = loopmin; i<loopsize; i = i<<1) {
                ptrtimer_reset(t0);
                for (j = 0; j < loopsize / i; j++) {
                        pthread_barrier_init (&barrier1, NULL, Nthreads+1);
                        pthread_barrier_init (&barrier2, NULL, Nthreads+1);
                        for(k = 0; k < Nthreads; k++) {
                                (function_args[k])->x = x;
                                (function_args[k])->start = (i/Nthreads)*k;
                                (function_args[k])->stop  = (i/Nthreads)*(k+1);
                        }
                        for(k = 0; k < Nthreads; k++)
                                pthread_create(&(tid[k]),NULL,out1in1flop1mul1add,(void *)(function_args[k]));
                        ptrtimer_start(t0);
                        pthread_barrier_wait (&barrier1);
                        pthread_barrier_wait (&barrier2);
                        ptrtimer_stop(t0);
                        for(k = 0; k < Nthreads; k++)
                                pthread_join(tid[k],NULL);
                        pthread_barrier_destroy(&barrier1);
                        pthread_barrier_destroy(&barrier2);                    
                
                }
                printf("size=%ld rep=%ld Mflop/s=%4.3f MByte/s=%4.3f \n",i, loopsize/i,
                        (double)i / ptrtimer_getavg(t0) * 2.0 / 1000000.0,
                        (double)i / ptrtimer_getavg(t0) * 2 * sizeof(float) / 1000000.0);
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
