#include <cuda_runtime.h>
#include <helper_cuda.h>
#include "myProto.h"

// For the GPU to recognize this function
__device__ double func(int i)
{
    double value;
    double result = 0;

    for (int j = 1; j < HEAVY; j++)
    {
        value = (i + 1) * (j % 10);
        result += cos(value);
    }
    return cos(result);
}

// each thread has arr,numElements his own.
__global__ void fFunction(int *arr, int numElements, int *counter)
{

    int i = blockDim.x * blockIdx.x + threadIdx.x;
    if (i < numElements)
        if (func(arr[i]) > 0) // we will update the arr when we have func(arr[i])>0 so we will
            atomicAdd(&counter[0], 1);
    __syncthreads();
}

int computeOnGPU(int *data, int numElements, int *counter)
{
    // Error code to check return values for CUDA calls
    cudaError_t err = cudaSuccess;

    size_t size = numElements * sizeof(int);
    size_t sizeCounter = sizeof(int);
    // Allocate memory on GPU to copy the data from the host
    int *d_A;
    int *d_counter;

    err = cudaMalloc((void **)&d_A, size);
    if (err != cudaSuccess)
    {
        fprintf(stderr, "Failed to allocate device memory - %s\n", cudaGetErrorString(err));
        exit(EXIT_FAILURE);
    }

    err = cudaMalloc((void **)&d_counter, sizeCounter);
    if (err != cudaSuccess)
    {
        fprintf(stderr, "Failed to allocate device memory - %s\n", cudaGetErrorString(err));
        exit(EXIT_FAILURE);
    }

    // Copy data from host to the GPU memory
    err = cudaMemcpy(d_A, data, size, cudaMemcpyHostToDevice);
    if (err != cudaSuccess)
    {
        fprintf(stderr, "Failed to copy data from host to device - %s\n", cudaGetErrorString(err));
        exit(EXIT_FAILURE);
    }

    // Copy data from host to the GPU memory
    err = cudaMemcpy(d_counter, counter, sizeCounter, cudaMemcpyHostToDevice);
    if (err != cudaSuccess)
    {
        fprintf(stderr, "Failed to copy data from host to device - %s\n", cudaGetErrorString(err));
        exit(EXIT_FAILURE);
    }

    // Launch the Kernel
    int threadsPerBlock = 256;
    int blocksPerGrid = (numElements + threadsPerBlock - 1) / threadsPerBlock;
    fFunction<<<blocksPerGrid, threadsPerBlock>>>(d_A, numElements, d_counter);
    err = cudaGetLastError();
    if (err != cudaSuccess)
    {
        fprintf(stderr, "Failed to launch vectorAdd kernel -  %s\n", cudaGetErrorString(err));
        exit(EXIT_FAILURE);
    }

    // Copy data from GPU to the HOST memory
    err = cudaMemcpy(counter, d_counter, sizeCounter, cudaMemcpyDeviceToHost);
    if (err != cudaSuccess)
    {
        fprintf(stderr, "Failed to copy data from host to device - %s\n", cudaGetErrorString(err));
        exit(EXIT_FAILURE);
    }

    // Free allocated memory on GPU
    if (cudaFree(d_A) != cudaSuccess)
    {
        fprintf(stderr, "Failed to free device data - %s\n", cudaGetErrorString(err));
        exit(EXIT_FAILURE);
    }

    // Free allocated memory on GPU
    if (cudaFree(d_counter) != cudaSuccess)
    {
        fprintf(stderr, "Failed to free device data - %s\n", cudaGetErrorString(err));
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}
