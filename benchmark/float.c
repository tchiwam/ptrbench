#include<ptrtimer.h>
#include<stdlib.h>
#include<stdio.h>
#include<config.h>

#define TOTALSIZE 512000000
// 4k=12 8k=13 16k=14 32k=15 64k=16 128k=17 256k=18 512k=19 
// 1M=20 2M=21 4M=22 8M=23 16M=24 32M=25 64M=26 128M=27 256M=28 512M=29 
// 1G=30
#define LOOPMIN 1 << 12 
#define LOOPMAX 1 << 29
#define THREADMIN 1
#define THREADMAX 8
int main()
{
// Generic variables
long i,j,k,ilim;
long loopmin = LOOPMIN, loopmax = LOOPMAX , loopsize;
float *x,*a,*b;

ptrtimer *t0;
t0 = ptrtimer_init(10);


printf("Compiler " ptrbench_C_COMPILER_ID "\n Version " ptrbench_C_COMPILER_VERSION "\n CFLAGS " ptrbench_CFLAGS "\n");


// Adjust the loop parameters
loopsize = (TOTALSIZE/sizeof(float))/3;
printf("Total memory allocated %ld Mbytes\n",loopsize*3*sizeof(float)/(1024*1024));
printf("Loop size              %ld Mfloats\n",loopsize/(1024*1024));

if (loopmin > loopsize)
   loopmin = loopsize;
if (loopmax > loopsize)
   loopmax = loopsize;

printf("Loop min size          %ld floats\n",loopmin);
printf("Loop max size          %ld floats\n",loopmax);

// Allocate arrays
x = malloc(loopsize*sizeof(float));
a = malloc(loopsize*sizeof(float));
b = malloc(loopsize*sizeof(float));

ptrtimer_start(t0);
for(k=0;k<loopsize;k++)
{
   a[k]=(float)k/0.5;
   b[k]=(float)k*1.7;
   x[k]=(float)k*-0.6;
}
ptrtimer_stop(t0);
ptrtimer_report(t0);

printf("1flop 1xfloat\n");
ptrtimer_reset(t0);
ilim = loopmin; 
for (i = loopmin; i<loopsize; i=i << 1)
{
   for (j=0;j<loopsize/i;j++)
   {
      ptrtimer_start(t0);
      for (k=0;k<i;k++)
      {
         x[k]*=-1.07;
      }
      ptrtimer_stop(t0);
   }
   printf("size=%ld rep=%ld Mflop/s=%4.3f MByte/s=%4.3f \n",i,loopsize/i,(double)i/ptrtimer_getavg(t0)*1.0/1000000.0,(double)i/ptrtimer_getavg(t0)*4.0/1000000.0);
   ptrtimer_reset(t0);
}

printf("1flop 2xfloat\n");
ptrtimer_reset(t0);
ilim = loopmin; 
for (i = loopmin; i<loopsize; i=i << 1)
{
   for (j=0;j<loopsize/i;j++)
   {
      ptrtimer_start(t0);
      for (k=0;k<i;k++)
      {
         x[k]=a[k]*1.09;
      }
      ptrtimer_stop(t0);
   }
   printf("size=%ld rep=%ld Mflops=%4.3f MByte/s=%4.3f \n",i,loopsize/i,(double)i/ptrtimer_getavg(t0)*1.0/1000000.0,(double)i/ptrtimer_getavg(t0)*8.0/1000000.0);
   ptrtimer_reset(t0);
}

printf("1flop 3xfloat\n");
ptrtimer_reset(t0);
ilim = loopmin; 
for (i = loopmin; i<loopsize; i=i << 1)
{
   for (j=0;j<loopsize/i;j++)
   {
      ptrtimer_start(t0);
      for (k=0;k<i;k++)
      {
         x[k]=a[k]*b[k];
      }
      ptrtimer_stop(t0);
   }
   printf("size=%ld rep=%ld Mflops=%4.3f MByte/s=%4.3f \n",i,loopsize/i,(double)i/ptrtimer_getavg(t0)*1.0/1000000.0,(double)i/ptrtimer_getavg(t0)*12.0/1000000.0);
   ptrtimer_reset(t0);
}

printf("4flop 3xfloat\n");
ptrtimer_reset(t0);
ilim = loopmin; 
for (i = loopmin; i<loopsize; i=i << 1)
{
   for (j=0;j<loopsize/i;j++)
   {
      ptrtimer_start(t0);
      for (k=0;k<i;k++)
      {
         x[k]=3.4*(a[k]*b[k]+1.05)+1.6;
      }
      ptrtimer_stop(t0);
   }
   printf("size=%ld rep=%ld Mflops=%4.3f MByte/s=%4.3f \n",i,loopsize/i,(double)i/ptrtimer_getavg(t0)*4.0/1000000.0,(double)i/ptrtimer_getavg(t0)*12.0/1000000.0);
   ptrtimer_reset(t0);
}

printf("6flop 3xfloat\n");
ptrtimer_reset(t0);
ilim = loopmin; 
for (i = loopmin; i<loopsize; i=i << 1)
{
   for (j=0;j<loopsize/i;j++)
   {
      ptrtimer_start(t0);
      for (k=0;k<i;k++)
      {
         x[k]=-0.7*(3.4*(a[k]*b[k]+1.05)+1.6)+0.3;
      }
      ptrtimer_stop(t0);
   }
   printf("size=%ld rep=%ld Mflops=%4.3f MByte/s=%4.3f \n",i,loopsize/i,(double)i/ptrtimer_getavg(t0)*6.0/1000000.0,(double)i/ptrtimer_getavg(t0)*12.0/1000000.0);
   ptrtimer_reset(t0);
}

printf("8flop 3xfloat\n");
ptrtimer_reset(t0);
ilim = loopmin; 
for (i = loopmin; i<loopsize; i=i << 1)
{
   for (j=0;j<loopsize/i;j++)
   {
      ptrtimer_start(t0);
      for (k=0;k<i;k++)
      {
         x[k]=5.3*(-0.7*(3.4*(a[k]*b[k]+1.05)+1.6)+0.3)+1.4;
      }
      ptrtimer_stop(t0);
   }
   printf("size=%ld rep=%ld Mflops=%4.3f MByte/s=%4.3f \n",i,loopsize/i,(double)i/ptrtimer_getavg(t0)*8.0/1000000.0,(double)i/ptrtimer_getavg(t0)*12.0/1000000.0);
   ptrtimer_reset(t0);
}

printf("10flop 3xfloat\n");
ptrtimer_reset(t0);
ilim = loopmin; 
for (i = loopmin; i<loopsize; i=i << 1)
{
   for (j=0;j<loopsize/i;j++)
   {
      ptrtimer_start(t0);
      for (k=0;k<i;k++)
      {
         x[k]=0.4*(5.3*(-0.7*(3.4*(a[k]*b[k]+1.05)+1.6)+0.3)+1.4)+0.1;
      }
      ptrtimer_stop(t0);
   }
   printf("size=%ld rep=%ld Mflops=%4.3f MByte/s=%4.3f \n",i,loopsize/i,(double)i/ptrtimer_getavg(t0)*10.0/1000000.0,(double)i/ptrtimer_getavg(t0)*12.0/1000000.0);
   ptrtimer_reset(t0);
}
printf("12flop 3xfloat\n");
ptrtimer_reset(t0);
ilim = loopmin; 
for (i = loopmin; i<loopsize; i=i << 1)
{
   for (j=0;j<loopsize/i;j++)
   {
      ptrtimer_start(t0);
      for (k=0;k<i;k++)
      {
         x[k]=1.3*(0.4*(5.3*(-0.7*(3.4*(a[k]*b[k]+1.05)+1.6)+0.3)+1.4)+0.1)+1.3;
      }
      ptrtimer_stop(t0);
   }
   printf("size=%ld rep=%ld Mflops=%4.3f MByte/s=%4.3f \n",i,loopsize/i,(double)i/ptrtimer_getavg(t0)*12.0/1000000.0,(double)i/ptrtimer_getavg(t0)*12.0/1000000.0);
   ptrtimer_reset(t0);
}


ptrtimer_close(t0);
free(b);
free(a);
free(x);

return 0;
}
