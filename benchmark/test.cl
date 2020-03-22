__kernel void vector_add(__global const float *A, __global const float *B, __global float *C) {
	// Get the index of the current element to be processed
    	int i = get_global_id(0);
    	// Do the operation
    	C[i] = A[i] + B[i];
}
__kernel void vector_1add1float(__global float *A) {
    	int i = get_global_id(0);
    	A[i] += -1.07;
}

__kernel void vector_1mul1float(__global float *A) {
    	int i = get_global_id(0);
    	A[i] *= -1.07;
}

__kernel void vector_1mul1add1float(__global float *A) {
    	int i = get_global_id(0);
    	A[i] = A[i]*1.02-1.07;
}


