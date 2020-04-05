#include<iostream>
#include<algorithm>
#include<omp.h>
#include<chrono>
#include <thrust/host_vector.h>
#include <thrust/device_vector.h>
#include <thrust/generate.h>
#include <thrust/set_operations.h>
#include <thrust/sort.h>
#include <thrust/copy.h>
#include "general.cuh"
//using namespace std;
using ::std::cout;
using ::std::endl;
void basicSort(int * col1, int * col2, int c1, int c2){
    int cnt = 0;
    std::sort(col1, col1 + c1);
    std::sort(col2, col2 + c2);
    int i = 0;
    int j = 0;
    for(; i < c1 && j < c2;){
        if (col1[i] < col2[j]){
            i += 1;
        }
        else if (col1[i] > col2[j]){
            j += 1;
        } else {
            i += 1;
            j += 1;
            cnt += 1;     
        }
    }
    cout << cnt << endl;
}

__global__ void partitionKernelSort(int * c1, int * c2,int n1, int n2, int partitions, int blocks, CustomContainer * containers1, CustomContainer * containers2){
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

void sortParallel(int * c1, int * c2, int n1, int n2, int partitions){
    
    // cudaEvent_t start, stop;
    // cudaEventCreate(&start);
    // cudaEventCreate(&stop);
    float milliseconds = 0;
    // cudaEventRecord(start);
    
    thrust::device_vector<int> dCol1(c1, c1 + n1);
    thrust::device_vector<int> dCol2(c2, c2 + n2);
    thrust::sort(dCol1.begin(), dCol1.end());
    thrust::sort(dCol2.begin(), dCol2.end());
    
    thrust::device_vector<int> result(n1);
    // int * result = 0;
    // cudaMalloc(&result, sizeof(int) * n1);
    thrust::device_vector<int>::iterator last = thrust::set_intersection(dCol1.begin(), dCol1.end(), dCol2.begin(), dCol2.end(), result.begin());
    
    //cudaThreadSynchronize();
    int match = thrust::distance(result.begin(), last);
    
    // cudaEventRecord(stop);
    // cudaEventSynchronize(stop);
    
    // cudaEventElapsedTime(&milliseconds, start, stop);
    
    cout << "Matched - " <<  match << endl;
}


void partitionedSortParallel(int * c1, int * c2, int n1, int n2, int partitions) { 
    cudaEvent_t start, stop;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);
    cudaEventRecord(start);
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
        partitionKernelSort<<<blocks, 256>>>(thrust::raw_pointer_cast(dCol1.data()),thrust::raw_pointer_cast(dCol2.data()),n1,n2, partitions, blocks, deviceContainers1, deviceContainers2);
    } catch (thrust::system_error e){
        std::cout << "Error: " << e.what() << std::endl;
    }
    cudaThreadSynchronize();
    thrust::device_vector<int> result(n1);
    thrust::device_vector<int>::iterator first = result.begin();
    thrust::device_vector<int>::iterator last_temp = first;
    for(int p = 0; p < partitions; p++){
        int * col1 = containers1->getPartition(p);
        int * col2 = containers2->getPartition(p);
        int n1 = containers1->getPartitionSize(p);
        int n2 = containers2->getPartitionSize(p);
        
        thrust::device_vector<int> dCol1(col1, col1+n1);
        thrust::device_vector<int> dCol2(col2, col2 +n2);
        thrust::sort(dCol1.begin(), dCol1.end());
        thrust::sort(dCol2.begin(), dCol2.end());
        thrust::device_vector<int>::iterator last = thrust::set_intersection(dCol1.begin(), dCol1.end(), dCol2.begin(), dCol2.end(), last_temp);
        last_temp = last;
    }
    //cudaThreadSynchronize();
    cudaEventRecord(stop);
    float milliseconds = 0;
    cudaEventSynchronize(stop);
    cudaEventElapsedTime(&milliseconds, start, stop);
    cout << "Time elapsed: " << milliseconds << std::endl;
    cout << "Matched - " << thrust::distance(result.begin(), last_temp) << endl;
}