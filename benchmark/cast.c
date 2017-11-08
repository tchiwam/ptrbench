#include<ptrtimer.h>
#include<stdlib.h>
#include<stdio.h>

#define TOTALSIZE 1024000000*2
// 4k=12 8k=13 16k=14 32k=15 64k=16 128k=17 256k=18 512k=19 
// 1M=20 2M=21 4M=22 8M=23 16M=24 32M=25 64M=26 128M=27 256M=28 512M=29 
// 1G=30
#define LOOPMIN 1 << 14 
#define LOOPMAX 1 << 29
#define THREADMIN 1
#define THREADMAX 8
main()
{
// Generic variables
long i,j,k,ilim;
long long loopmin = LOOPMIN, loopmax = LOOPMAX , loopsize;
float *x;
unsigned char  *a;
unsigned short *b;
unsigned long  *c;
float *lu8,*lu16;

ptrtimer *t0;
t0 = ptrtimer_init(10);

// Adjust the loop parameters
loopsize = (TOTALSIZE/sizeof(float))/3;
printf("Total memory allocated %lld Mbytes\n",loopsize*3*sizeof(float)/(1024*1024));
printf("Loop size              %lld Mfloats\n",loopsize/(1024*1024));

if (loopmin > loopsize)
   loopmin = loopsize;
if (loopmax > loopsize)
   loopmax = loopsize;

printf("Loop min size          %lld floats\n",loopmin);
printf("Loop max size          %lld floats\n",loopmax);

// Allocate arrays
x = malloc(loopsize*sizeof(float));
a = malloc(loopsize*sizeof(unsigned char));
b = malloc(loopsize*sizeof(unsigned short));
c = malloc(loopsize*sizeof(unsigned long));
printf("Size lu8 %d\n",1<<8);
lu8  = malloc((1<<8 )*sizeof(float));
lu16 = malloc((1<<16)*sizeof(float));

printf("Location of arrays\n");
printf("a[]=%p\n",a);
printf("b[]=%p\n",b);
printf("c[]=%p\n",c);
printf("x[]=%p\n",x);
printf("lu8[] =%p\n",lu8);
printf("lu16[]=%p\n",lu16);

printf("lu8 generation\n");
ptrtimer_start(t0);
for(k=0;k<(1<<8);k++)
{
   lu8[k]=(float)k;
}
ptrtimer_stop(t0);
ptrtimer_report(t0);
ptrtimer_reset(t0);

printf("lu16 generation\n");
ptrtimer_start(t0);
for(k=0;k<(1<<16);k++)
{
   lu16[k]=(float)k;
}
ptrtimer_stop(t0);
ptrtimer_report(t0);
ptrtimer_reset(t0);

printf("filling of arrays\n");
ptrtimer_start(t0);
for(k=0;k<loopsize;k++)
{
   a[k]=(unsigned char)  k & 0x000000FF;
   b[k]=(unsigned short) k & 0x0000FFFF;
   c[k]=(unsigned long)  k & 0xFFFFFFFF;
}
ptrtimer_stop(t0);
ptrtimer_report(t0);
ptrtimer_reset(t0);


ilim = loopmin; 

printf("Simple cast unsigned char to float\n");
for (i = loopmin; i<loopsize; i=i << 1)
{
   for (j=0;j<loopsize/i;j++)
   {
      ptrtimer_start(t0);
      for (k=0;k<i;k++)
      {
         x[k]=(float)a[k];
      }
      ptrtimer_stop(t0);
   }
   printf("size=%ld rep=%lld Mcast/s=%4.3f MByte/s=%4.3f \n",i,loopsize/i,(double)i/ptrtimer_getavg(t0)*1.0/1000000.0,(double)i/ptrtimer_getavg(t0)*5.0/1000000.0);
   ptrtimer_reset(t0);
}

printf("Simple cast unsigned short to float\n");
for (i = loopmin; i<loopsize; i=i << 1)
{
   for (j=0;j<loopsize/i;j++)
   {
      ptrtimer_start(t0);
      for (k=0;k<i;k++)
      {
         x[k]=(float)b[k];
      }
      ptrtimer_stop(t0);
   }
   printf("size=%ld rep=%lld Mcast/s=%4.3f MByte/s=%4.3f \n",i,loopsize/i,(double)i/ptrtimer_getavg(t0)*1.0/1000000.0,(double)i/ptrtimer_getavg(t0)*6.0/1000000.0);
   ptrtimer_reset(t0);
}

printf("Simple cast unsigned long to float\n");
for (i = loopmin; i<loopsize; i=i << 1)
{
   for (j=0;j<loopsize/i;j++)
   {
      ptrtimer_start(t0);
      for (k=0;k<i;k++)
      {
         x[k]=(float)c[k];
      }
      ptrtimer_stop(t0);
   }
   printf("size=%ld rep=%lld Mcast/s=%4.3f MByte/s=%4.3f \n",i,loopsize/i,(double)i/ptrtimer_getavg(t0)*1.0/1000000.0,(double)i/ptrtimer_getavg(t0)*8.0/1000000.0);
   ptrtimer_reset(t0);
}

printf("lu8 cast unsigned char to float\n");
for (i = loopmin; i<loopsize; i=i << 1)
{
   for (j=0;j<loopsize/i;j++)
   {
      ptrtimer_start(t0);
      for (k=0;k<i;k++)
      {
         x[k]=lu8[a[k]];
      }
      ptrtimer_stop(t0);
   }
   printf("size=%ld rep=%lld Mcast/s=%4.3f MByte/s=%4.3f \n",i,loopsize/i,(double)i/ptrtimer_getavg(t0)*1.0/1000000.0,(double)i/ptrtimer_getavg(t0)*5.0/1000000.0);
   ptrtimer_reset(t0);
}

printf("lu16 cast unsigned short to float\n");
for (i = loopmin; i<loopsize; i=i << 1)
{
   for (j=0;j<loopsize/i;j++)
   {
      ptrtimer_start(t0);
      for (k=0;k<i;k++)
      {
         x[k]=lu16[b[k]];
      }
      ptrtimer_stop(t0);
   }
   printf("size=%ld rep=%lld Mcast/s=%4.3f MByte/s=%4.3f \n",i,loopsize/i,(double)i/ptrtimer_getavg(t0)*1.0/1000000.0,(double)i/ptrtimer_getavg(t0)*6.0/1000000.0);
   ptrtimer_reset(t0);
}

printf("Simple cast unsigned char to float mx+b\n");
for (i = loopmin; i<loopsize; i=i << 1)
{
   for (j=0;j<loopsize/i;j++)
   {
      ptrtimer_start(t0);
      for (k=0;k<i;k++)
      {
         x[k]=(float)a[k]*0.1+3.2;
      }
      ptrtimer_stop(t0);
   }
   printf("size=%ld rep=%lld Mcast/s=%4.3f MByte/s=%4.3f \n",i,loopsize/i,(double)i/ptrtimer_getavg(t0)*1.0/1000000.0,(double)i/ptrtimer_getavg(t0)*5.0/1000000.0);
   ptrtimer_reset(t0);
}

printf("Simple cast unsigned short to float mx+b\n");
for (i = loopmin; i<loopsize; i=i << 1)
{
   for (j=0;j<loopsize/i;j++)
   {
      ptrtimer_start(t0);
      for (k=0;k<i;k++)
      {
         x[k]=(float)b[k]*0.1+3.2;
      }
      ptrtimer_stop(t0);
   }
   printf("size=%ld rep=%lld Mcast/s=%4.3f MByte/s=%4.3f \n",i,loopsize/i,(double)i/ptrtimer_getavg(t0)*1.0/1000000.0,(double)i/ptrtimer_getavg(t0)*6.0/1000000.0);
   ptrtimer_reset(t0);
}

printf("lu8 cast unsigned char to float mx+b\n");
for (i = loopmin; i<loopsize; i=i << 1)
{
   for (j=0;j<loopsize/i;j++)
   {
      ptrtimer_start(t0);
      for(k=0;k<(1<<8);k++)
         lu8[k] = lu8[k]*0.1+3.2;
      for (k=0;k<i;k++)
      {
         x[k]=lu8[a[k]];
      }
      ptrtimer_stop(t0);
   }
   printf("size=%ld rep=%lld Mcast/s=%4.3f MByte/s=%4.3f \n",i,loopsize/i,(double)i/ptrtimer_getavg(t0)*1.0/1000000.0,(double)i/ptrtimer_getavg(t0)*5.0/1000000.0);
   ptrtimer_reset(t0);
}

printf("lu16 cast unsigned short to float mx+b\n");
for (i = loopmin; i<loopsize; i=i << 1)
{
   for (j=0;j<loopsize/i;j++)
   {
      ptrtimer_start(t0);
      for(k=0;k<(1<<16);k++)
         lu16[k] = lu16[k]*0.1+3.2;
      for (k=0;k<i;k++)
      {
         x[k]=lu16[b[k]];
      }
      ptrtimer_stop(t0);
   }
   printf("size=%ld rep=%lld Mcast/s=%4.3f MByte/s=%4.3f \n",i,loopsize/i,(double)i/ptrtimer_getavg(t0)*1.0/1000000.0,(double)i/ptrtimer_getavg(t0)*6.0/1000000.0);
   ptrtimer_reset(t0);
}

ptrtimer_close(t0);

printf("Freeing array a\n");
free(a);
printf("Freeing array b\n");
free(b);
printf("Freeing array c\n");
free(c);
printf("Freeing array x\n");
free(x);
printf("Freeing array lu8\n");
free(lu8);
printf("Freeing array lu16\n");
free(lu16);

return 0;
}
