#include "general.cuh"
#include <stdio.h>
#include<iostream>
#include <thrust/host_vector.h>
#include <thrust/device_vector.h>
#include <thrust/generate.h>
#include <thrust/set_operations.h>
#include <thrust/sort.h>
#include <thrust/copy.h>
#include <stdio.h>

__global__ void partitionKernelHash(int * c1, int * c2,int n1, int n2, int partitions, int blocks, CustomContainer * containers1, CustomContainer * containers2){
    int blockSize1 = n1 / blocks;
    int p;
    int idx = blockSize1 * blockIdx.x + threadIdx.x;
    for (int i = idx; i < blockSize1 * (blockIdx.x + 1) && i < n1; i += blockDim.x){
        p = c1[i] % partitions;
        containers1->push_back(p, 0, c1[i]);
    }
    int blockSize2 = n2 / blocks;
    idx = blockSize2 * blockIdx.x + threadIdx.x;
    for (int i = idx; i < blockSize2 * (blockIdx.x + 1) && i < n2; i += blockDim.x){
        p = c2[i] % partitions;
        containers2->push_back(p, 0, c2[i]);
    }
}
__global__ void probe(CustomContainer * container1, CustomContainer * container2, int partitions, int * finalCount){
    __shared__ int countArr[256];
    int * col1 = container1->getPartition(blockIdx.x);
    int * col2 = container2->getPartition(blockIdx.x);
    int n1 = container1->getPartitionSize(blockIdx.x);
    int n2 = container2->getPartitionSize(blockIdx.x);
    int idx = threadIdx.x;
    int count = 0;
    for(int i = idx; i < n1; i += blockDim.x){
        for (int j = 0; j < n2; j++){
            count += (col1[i] == col2[j]);
        }
    }
    countArr[threadIdx.x] = count;
    __syncthreads();
    if(threadIdx.x == 0){
        count = 0;
        for(int i = 0; i < blockDim.x; i++){
            count += countArr[i];
        }
        //printf("%d\n", count);
        atomicAdd(finalCount, count);
    }
}

void partitionedHash(int * c1, int * c2, int n1, int n2, int partitions, bool serial) { 
    thrust::device_vector<int> dCol1(c1, c1 + n1);
    thrust::device_vector<int> dCol2(c2, c2 + n2);
    int blocks = 80;
    int blockSize = n1/1;
    CustomContainer * containers1 = new CustomContainer(2 * n1/partitions, partitions, 1, NULL);
    CustomContainer * containers2 = new CustomContainer(2 * n2/partitions, partitions, 1, NULL);
    CustomContainer * deviceContainers1 = 0;
    CustomContainer * deviceContainers2 = 0;
    cudaMalloc(&deviceContainers1, sizeof(CustomContainer));
    cudaMalloc(&deviceContainers2, sizeof(CustomContainer));
    cudaMemcpy(&deviceContainers1->container, &containers1->container, sizeof(int *), cudaMemcpyHostToDevice);
    cudaMemcpy(&deviceContainers1->meta, &containers1->meta, sizeof(int *), cudaMemcpyHostToDevice);
    cudaMemcpy(&deviceContainers1->head, &containers1->head, sizeof(int *), cudaMemcpyHostToDevice);
    cudaMemcpy(&deviceContainers2->container, &containers2->container, sizeof(int *), cudaMemcpyHostToDevice);
    cudaMemcpy(&deviceContainers2->meta, &containers2->meta, sizeof(int *), cudaMemcpyHostToDevice);
    cudaMemcpy(&deviceContainers2->head, &containers2->head, sizeof(int *), cudaMemcpyHostToDevice);
    int * finalCount = 0;
    cudaMalloc(&finalCount, sizeof(int)*partitions);
    try{
        partitionKernelHash<<<blocks, 256>>>(thrust::raw_pointer_cast(dCol1.data()),thrust::raw_pointer_cast(dCol2.data()),n1,n2, partitions, blocks, deviceContainers1, deviceContainers2);
    } catch (thrust::system_error e){
        std::cout << "Error: " << e.what() << std::endl;
    }
    cudaThreadSynchronize();
    probe<<<partitions, 256>>>(deviceContainers1,deviceContainers2,partitions, finalCount); 
    cudaThreadSynchronize();
    int finalCountHost = 0;
    cudaMemcpy(&finalCountHost, finalCount, sizeof(int), cudaMemcpyDeviceToHost);
    printf("Final Count: %d\n", finalCountHost);
}