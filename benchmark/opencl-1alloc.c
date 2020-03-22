#define _GNU_SOURCE
#include<ptrtimer.h>
#include<stdio.h>
#include<sys/stat.h>
#include<CL/cl.h>
#include<config.h>

#define TOTALSIZE 2048000000
// 1k=10 2k=11 4k=12 8k=13 16k=14 32k=15 64k=16 128k=17 256k=18 512k=19 
// 1M=20 2M=21 4M=22 8M=23 16M=24 32M=25 64M=26 128M=27 256M=28 512M=29 
// 1G=30 ...
#define LOOPMIN  10
#define LOOPMAX  31
#define THREADMIN 1
#define THREADMAX 8

int main(void)
{
        float *x; 
        unsigned long i, j, k, ilim;
        unsigned long loopmin = 1UL << LOOPMIN; 
        unsigned long loopmax = 1UL << LOOPMAX;
        unsigned long loopsize;

        ptrtimer *t0;
        t0 = ptrtimer_init(10);

        printf("Compiler " ptrbench_C_COMPILER_ID "\n Version " ptrbench_C_COMPILER_VERSION "\n CFLAGS " ptrbench_CFLAGS "\n");
   
        // Adjust the loop parameters
        loopsize = (TOTALSIZE/sizeof(float))/1;
        printf("Total memory allocated %ld Mbytes \n", loopsize*1*sizeof(float)/(1024*1024));
        printf("Loop size              %ld Mfloats\n", loopsize/(1024*1024));

        if (loopmin > loopsize) {
                printf("Adjusting loopsize to loopmin\n");
                loopmin = loopsize;
        }
        if (loopmax > loopsize) {
                printf("Adjusting loopsize to loopmax\n");
                loopmax = loopsize;
        }

        printf("Loop min size          %ld floats\n", loopmin);
        printf("Loop max size          %ld floats\n", loopmax);

        // Allocate arrays
        x = malloc(loopsize*sizeof(float));

        FILE *file;
        char *source_cl;
        size_t source_cl_size, source_cl_size_read;
        char *filename;
        struct stat fileinfo;
        /* Loading the OpenCL source here */
        printf("Loading OpenCL\n");
        if (asprintf(&filename, "%s", "test.cl") < 0) {
                printf("Failed to create file path");
                exit(1);
        }
        stat(filename, &fileinfo);
        source_cl_size = fileinfo.st_size;
        source_cl = (char*)malloc(source_cl_size);
        file = fopen(filename, "r");
        if (!file) {
                printf("Failed to load OpenCL source : %s\n", filename);
                exit(1);
        }
        source_cl_size_read = fread(source_cl, 1, source_cl_size, file);
        if (source_cl_size_read != source_cl_size) {
                printf("Size missmatch for source : %s\n", filename);
                printf("Size missmatch for source : %ld != %ld\n", source_cl_size, source_cl_size_read);
                exit(1);
        }
        fclose(file);

        /* Platform information */
        printf("Platform information\n");
        cl_platform_id platform_id = NULL;
        cl_device_id device_id = NULL;
        cl_uint ret_num_devices = 0;
        cl_uint ret_num_platforms = 0;
        cl_int ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
        if (ret != CL_SUCCESS) {
                printf("clGetPlatformIDs %d\n", ret);
                exit(1);
        }
        printf("Num platforms %d\n", ret_num_platforms);
        ret = clGetDeviceIDs( platform_id, CL_DEVICE_TYPE_DEFAULT, 1, &device_id, &ret_num_devices);
        if (ret != CL_SUCCESS) {
                printf("clGetDeviceIDs %d\n", ret);
                exit(1);
        }
        cl_context context = clCreateContext( NULL, 1, &device_id, NULL, NULL, &ret);
        cl_command_queue command_queue = clCreateCommandQueueWithProperties(context, device_id, 0, &ret);
        printf("clCreateCommandQueueWithProperties %d\n", ret);

        cl_mem a_cl = clCreateBuffer(context, CL_MEM_READ_ONLY, loopsize * sizeof(float), NULL, &ret);
        if (ret != CL_SUCCESS) {
                printf("lCreateBuffer a %d\n", ret);
                exit(1);
        }
   
        cl_mem b_cl = clCreateBuffer(context, CL_MEM_READ_ONLY, loopsize * sizeof(float), NULL, &ret);
        if (ret != CL_SUCCESS) {
                printf("lCreateBuffer b %d\n", ret);
                exit(1);
        }
   
        cl_mem x_cl = clCreateBuffer(context, CL_MEM_READ_WRITE, loopsize * sizeof(float), NULL, &ret);
        if (ret != CL_SUCCESS) {
                printf("lCreateBuffer c %d\n", ret);
                exit(1);
        }
        // Make up some values
        for (i = 0;i < loopsize;i++) {
                x[i] = (float)i / loopsize;
        }
        
        // Create a program from the kernel source
        printf("Create program with source\n");
        cl_program program = clCreateProgramWithSource(context, 1, (const char **)&source_cl, (const size_t *)&source_cl_size, &ret);
        if (ret != CL_SUCCESS) {
                printf("clCreateProgramWithSource %d\n", ret);
                exit(1);
        }
        // Build the program
        ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
        if (ret != CL_SUCCESS) {
                printf("clBuildProgram %d\n", ret);
                exit(1);
        }

        cl_kernel kernel; 
        size_t global_item_size = loopsize; // Process the entire lists
        size_t local_item_size; // Divide work items into groups of 64
        
        // Benchmark starts here
        ptrtimer_reset(t0);
        printf("a[] *= m\n");
        ilim = loopmin;
        kernel = clCreateKernel(program, "vector_1mul1float", &ret);
        if( ret != CL_SUCCESS) {
                printf("clCreateKernel %d\n", ret);
                exit(1);
        }
        ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&x_cl);
        if( ret != CL_SUCCESS) {
                printf("clSetKernelArg %d\n", ret);
                exit(1);
        }
        ret = clGetKernelWorkGroupInfo(kernel, device_id, CL_KERNEL_WORK_GROUP_SIZE, sizeof(local_item_size), &local_item_size, NULL);
        if( ret != CL_SUCCESS) {
                printf("clGetKernelWorkGroupInfo %d\n", ret);
                exit(1);
        }
        printf("Local Size item %ld \n", local_item_size);
        for (i = loopmin; i<loopsize; i=i << 1) {
                for (j = 0;j < loopsize / i;j++) {
                        ptrtimer_start(t0);
                        global_item_size = i;
                        ret = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, &global_item_size, &local_item_size, 0, NULL, NULL);
                        if (ret != CL_SUCCESS) {
                                printf("clEnqueueNDRangeKernel %d\n", ret);
                                exit(1);
                        }
                        ret = clFinish(command_queue);
                        ptrtimer_stop(t0);
                        if (ret != CL_SUCCESS) {
                                printf("clFinish %d\n", ret);
                                exit(1);
                        }
                }
                printf("size=%ld rep=%ld Mflop/s=%4.3f MByte/s=%4.3f \n", i, loopsize / i, (double)i / ptrtimer_getavg(t0) * 1.0 / 1000000.0, (double)i / ptrtimer_getavg(t0) * 2.0 * sizeof(float) / 1000000.0);
                ptrtimer_reset(t0);
        }
        if (ret != CL_SUCCESS) {
                printf("clFlush %d\n", ret);
                exit(1);
        }
        ret = clFinish(command_queue);
        if (ret != CL_SUCCESS) {
                printf("clFinish %d\n", ret);
                exit(1);
        }
        if (ret != CL_SUCCESS) {
                printf("clReleaseKernel %d\n", ret);
                exit(1);
        }        
        
        // Benchmark starts here
        ptrtimer_reset(t0);
        printf("a[] += b\n");
        ilim = loopmin;
        kernel = clCreateKernel(program, "vector_1add1float", &ret);
        if( ret != CL_SUCCESS) {
                printf("clCreateKernel %d\n", ret);
                exit(1);
        }
        ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&x_cl);
        if( ret != CL_SUCCESS) {
                printf("clSetKernelArg %d\n", ret);
                exit(1);
        }
        ret = clGetKernelWorkGroupInfo(kernel, device_id, CL_KERNEL_WORK_GROUP_SIZE, sizeof(local_item_size), &local_item_size, NULL);
        if( ret != CL_SUCCESS) {
                printf("clGetKernelWorkGroupInfo %d\n", ret);
                exit(1);
        }
        printf("Local Size item %ld \n", local_item_size);
        for (i = loopmin; i<loopsize; i=i << 1) {
                for (j = 0;j < loopsize / i;j++) {
                        ptrtimer_start(t0);
                        global_item_size = i;
                        ret = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, &global_item_size, &local_item_size, 0, NULL, NULL);
                        if (ret != CL_SUCCESS) {
                                printf("clEnqueueNDRangeKernel %d\n", ret);
                                exit(1);
                        }
                        ret = clFinish(command_queue);
                        ptrtimer_stop(t0);
                        if (ret != CL_SUCCESS) {
                                printf("clFinish %d\n", ret);
                                exit(1);
                        }
                }
                printf("size=%ld rep=%ld Mflop/s=%4.3f MByte/s=%4.3f \n", i, loopsize / i, (double)i / ptrtimer_getavg(t0) * 1.0 / 1000000.0, (double)i / ptrtimer_getavg(t0) * 2.0 * sizeof(float) / 1000000.0);
                ptrtimer_reset(t0);
        }
        if (ret != CL_SUCCESS) {
                printf("clFlush %d\n", ret);
                exit(1);
        }
        ret = clFinish(command_queue);
        if (ret != CL_SUCCESS) {
                printf("clFinish %d\n", ret);
                exit(1);
        }
        if (ret != CL_SUCCESS) {
                printf("clReleaseKernel %d\n", ret);
                exit(1);
        }

        // Benchmark starts here
        ptrtimer_reset(t0);
        printf("a[] = a[]*m+b\n");
        ilim = loopmin; 
        kernel = clCreateKernel(program, "vector_1mul1add1float", &ret);
        if( ret != CL_SUCCESS) {
                printf("clCreateKernel %d\n", ret);
                exit(1);
        }
        ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&x_cl);
        if( ret != CL_SUCCESS) {
                printf("clSetKernelArg %d\n", ret);
                exit(1);
        }
        ret = clGetKernelWorkGroupInfo(kernel, device_id, CL_KERNEL_WORK_GROUP_SIZE, sizeof(local_item_size), &local_item_size, NULL);
        if( ret != CL_SUCCESS) {
                printf("clGetKernelWorkGroupInfo %d\n", ret);
                exit(1);
        }
        printf("Local Size item %ld \n", local_item_size);
        for (i = loopmin; i<loopsize; i=i << 1) {
                for (j = 0;j < loopsize / i;j++) {
                        ptrtimer_start(t0);
                        global_item_size = i;
                        ret = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, &global_item_size, &local_item_size, 0, NULL, NULL);
                        if (ret != CL_SUCCESS) {
                                printf("clEnqueueNDRangeKernel %d\n", ret);
                                exit(1);
                        }
                        ret = clFinish(command_queue);
	                ptrtimer_stop(t0);
                        if (ret != CL_SUCCESS) {
                                printf("clFinish %d\n", ret);
                                exit(1);
                        }
                }
                printf("size=%ld rep=%ld Mflop/s=%4.3f MByte/s=%4.3f \n", i, loopsize / i, (double)i / ptrtimer_getavg(t0) * 2.0 / 1000000.0, (double)i / ptrtimer_getavg(t0) * 2.0 * sizeof(float) / 1000000.0);
                ptrtimer_reset(t0);
        }
        if (ret != CL_SUCCESS) {
                printf("clFlush %d\n", ret);
                exit(1);
        }
        ret = clFinish(command_queue);
        if (ret != CL_SUCCESS) {
                printf("clFinish %d\n", ret);
                exit(1);
        }
        if (ret != CL_SUCCESS) {
                printf("clReleaseKernel %d\n", ret);
                exit(1);
        }

        // Clean up and exit
        printf("Freeing all\n");
        ret = clReleaseProgram(program);
        if (ret != CL_SUCCESS) {
                printf("clReleaseProgram %d\n", ret);
                exit(1);
        }
        ret = clReleaseMemObject(a_cl);
        if (ret != CL_SUCCESS) {
                printf("clReleaseMemObject a_cl %d\n", ret);
                exit(1);
        }
        ret = clReleaseMemObject(b_cl);
        if (ret != CL_SUCCESS) {
                printf("clReleaseMemObject b_cl %d\n", ret);
                exit(1);
        }
        ret = clReleaseMemObject(x_cl);
        if (ret != CL_SUCCESS) {
                printf("clReleaseMemObject x_cl %d\n", ret);
                exit(1);
        }
        ret = clReleaseCommandQueue(command_queue); 
        if (ret != CL_SUCCESS) {
                printf("clReleaseCommandQueue %d\n", ret);
                exit(1);
        }
        ret = clReleaseContext(context);
        if (ret != CL_SUCCESS) {
                printf("clReleaseContent %d\n", ret);
                exit(1);
        }
        free(source_cl);
        free(filename);
        ptrtimer_close(t0);
        free(x);
}

