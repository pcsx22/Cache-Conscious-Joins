#include <stdio.h>
#define gpuErrchk(ans) { gpuAssert((ans), __FILE__, __LINE__); }
inline void gpuAssert(cudaError_t code, const char *file, int line, bool abort=true)
{
   if (code != cudaSuccess) 
   {
      printf("GPUassert: %s %s %d\n", cudaGetErrorString(code), file, line);
      if (abort) exit(code);
   }
}
class CustomContainer {
public:
    int * meta;
    int * bufSize;
    int * partitions;
    int * container;
    int * head;
    int * blocks;
    CustomContainer * parentContainer;
    __host__ __device__ CustomContainer(){
        // this->bufSize = NULL;
        // this->partitions = NULL;
        // this->container = NULL;
        // this->head = NULL;
        // this->meta = NULL;
    }
    __host__ __device__ CustomContainer(int size, int partitions, int blocks, CustomContainer * parent){
        cudaMallocManaged(&meta, sizeof(int) * 4);
        size = size * (3/2);
        cudaMemcpy(&meta[0], &size, sizeof(int), cudaMemcpyHostToDevice);
        cudaMemcpy(&meta[1], &partitions, sizeof(int), cudaMemcpyHostToDevice);
        cudaMemcpy(&meta[2], &blocks, sizeof(int), cudaMemcpyHostToDevice);
        int blockSize = partitions * size;
        cudaMemcpy(&meta[3], &blockSize, sizeof(int), cudaMemcpyHostToDevice);
        this->container = 0;
        this->head = 0;
        cudaMallocManaged(&this->head, sizeof(int) * partitions * blocks);
        gpuErrchk(cudaMalloc(&this->container, sizeof(int) * partitions * size * blocks));
        this->parentContainer = parent;
    }

    __device__ __host__ int getPartitions(){
        return meta[1];
    }

    __device__ __host__ int getBufSize(){
        return meta[0];
    }

    __device__ __host__ int getBlocks(){
        return meta[2];
    }

    __device__ __host__ int getBlockSize(){
        return meta[3];
    }
    
    __device__ void push_back(int partition, int block, int el){
        //int j = __sync_fetch_and_add(this->head + partition, 1);
        int headPtr = block * this->getPartitions() + partition;
        int j = atomicAdd(this->head + headPtr, 1);
        this->container[block * this->getBlockSize() + partition * this->getBufSize() + j] = el;
        
    }
    
    __device__ __host__ int * getPartition(int partition){
        return this->container + partition * this->getBufSize();
    }

    __device__ __host__ int getPartitionSize(int partition){
        return this->head[partition];
    }

    __device__ __host__ ~CustomContainer(){
        cudaFree(container);
        cudaFree(head);
        cudaFree(meta);
    }

};

