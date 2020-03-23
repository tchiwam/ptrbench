#include<ptrtimer.h>
#include<math.h>
#include<stdlib.h>
#include<stdio.h>
#include<config.h>
#include<fftw3.h>
#define TOTALSIZE 1024000000
// 4k=12 8k=13 16k=14 32k=15 64k=16 128k=17 256k=18 512k=19 
// 1M=20 2M=21 4M=22 8M=23 16M=24 32M=25 64M=26 128M=27 256M=28 512M=29 
// 1G=30
#define LOOPMIN 1 << 14 
#define LOOPMAX 1 << 30
#define THREADMIN 1
#define THREADMAX 8
int main(void)
{
	// Generic variables
	long i,j,k;
	long loopmin = LOOPMIN, loopmax = LOOPMAX , loopsize;
	fftw_complex *a, *b;
	fftw_plan p;

	printf("Compiler " ptrbench_C_COMPILER_ID "\n Version " ptrbench_C_COMPILER_VERSION "\n CFLAGS " ptrbench_CFLAGS "\n");

	srand(time(NULL)); /* Call me only once  */


	printf("Timer allocation\n");
	ptrtimer *t0;
	t0 = ptrtimer_init(10);
	if(t0 == NULL) {
   		printf("Cannot allocate timer\n");
   		return -1;
	}

	// Adjust the loop parameters
	loopsize = (TOTALSIZE/sizeof(fftw_complex))/2;
	printf("Total memory allocated %ld Mbytes\n",loopsize*3*sizeof(float)/(1024*1024));
	printf("Loop size              %ld Mfloats\n",loopsize/(1024*1024));

	if (loopmin > loopsize)
   		loopmin = loopsize;
	if (loopmax > loopsize)
   		loopmax = loopsize;

	printf("Loop min size          %ld floats\n",loopmin);
	printf("Loop max size          %ld floats\n",loopmax);

	/* Array setup */
	printf("Allocate a\n");
	ptrtimer_start(t0);
	a = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * loopsize);
	if (a == NULL) {
  	 	printf("Cannot allocate a\n");
   		return -1;
	}
	printf("Allocate b\n");
	b = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * loopsize);
	if (b == NULL) {
   		printf("Cannot allocate b\n");
   		return -1;
	}
	ptrtimer_stop(t0);
	ptrtimer_report(t0);

	printf("1d dft comple float 64 out of place\n");
	ptrtimer_reset(t0);
	for (i = loopmin; i<loopsize; i=i << 1) {
	   	p = fftw_plan_dft_1d(i, a, b, FFTW_FORWARD, FFTW_EXHAUSTIVE);
   		for (j=0;j<loopsize/i;j++) {
      			for  (k=0 ; k<i; k++) {
         			a[k][0] = (float)k/loopsize;
         			a[k][1] = (float)k/loopsize;
      			}
      			ptrtimer_start(t0);
      			fftw_execute(p); /* repeat as needed */
      			ptrtimer_stop(t0);
   		}
   		fftw_destroy_plan(p);
   		printf("size=%ld rep=%ld Mflop/s=%4.3f MByte/s=%4.3f \n",i,loopsize/i,
			(5*(double)i*log2((double)i))/ptrtimer_getavg(t0)/1000000.0,
			(double)i/ptrtimer_getavg(t0)*2*sizeof(fftw_complex)/1000000.0);
   		ptrtimer_reset(t0);
	}

	ptrtimer_close(t0);
	printf("Free b\n");
	fftw_free(b);
	printf("Free a\n");
	fftw_free(a);
	return 0;
}
