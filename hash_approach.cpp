#include <unordered_set>
#include<algorithm>
#include<iostream>
#include <chrono>
#include<omp.h>
#include "hash_approach.h"
#include "general.h"
using namespace std;
void basicHash(int * c1, int * c2, int n1, int n2){
    unordered_set<int> hashTable(n1);
    for(int i = 0; i <n1; i++){
        hashTable.insert(c1[i]);
    }
    int cnt = 0;
    for(int i = 0; i < n2; i++){
        if(hashTable.find(c2[i]) != hashTable.end()){
            cnt++;
        }
    }
    cout << "Matched: " << cnt << endl;
}

void hashPartition(int * c1, int * c2, int n1, int n2, int partitons, custom_container & buff1, custom_container & buff2){
    int p;
    for (int i = 0; i < n1; i++){
        p = c1[i] % partitons;
        buff1.push_back(p, c1[i]);
        buff1.manageWriteBuff(p);
    }
    buff1.flushAll();
    for (int i = 0; i < n2; i++){
        p = c2[i] % partitons;
        buff2.push_back(p, c2[i]);
        buff2.manageWriteBuff(p);
    }
    buff2.flushAll();
}
void probeTableSerial(custom_container &buff1, custom_container &buff2, int pSize, int partitions){
    unordered_set <int> exists(pSize);
    int cnt = 0;
    for(int i = 0; i < partitions; i++){
        int * col1 = buff1.getPartition(i);
        int s1 = buff1.getPartitionSize(i);
        //build hash table for col1
        for (int j = 0; j < s1; j++){
            exists.insert(col1[j]);
        }

        int * col2 = buff2.getPartition(i);
        int s2 = buff2.getPartitionSize(i);
        //probe for col2
        for (int j = 0; j < s2; j++){
            if (exists.find(col2[j]) != exists.end()){
                cnt++;
            }
        }
        exists.clear();
        // exists.resize(pSize);
        exists.reserve(s1);
        //exists.flush();
    }
        cout << "Matched: " << cnt << endl;
}

void probeTableParallel(custom_container &buff1, custom_container &buff2, int pSize, int partitions){
    vector<unordered_set<int>* >hList(threads);
    for(int i = 0; i < threads; i++){
        hList[i] = new unordered_set<int>(pSize);    
    }
    int cnt = 0;
    #pragma omp parallel for num_threads(threads) reduction(+: cnt)
    for(int i = 0; i < partitions; i++){
        int * col1 = buff1.getPartition(i);
        int s1 = buff1.getPartitionSize(i);
        //build hash table for col1
        int tId = omp_get_thread_num();
        for (int j = 0; j < s1; j++){
            hList[tId]->insert(col1[j]);
        }

        int * col2 = buff2.getPartition(i);
        int s2 = buff2.getPartitionSize(i);
        //probe for col2
        for (int j = 0; j < s2; j++){
            if (hList[tId]->find(col2[j]) != hList[tId]->end()){
                cnt++;
            }
        }
        hList[tId]->clear();
        hList[tId]->reserve(pSize);
    }
        cout << "Matched: " << cnt << endl;
}

void hashPartitionParallel(int * c1, int * c2, int n1, int n2, int partitions, custom_container * container1, custom_container * container2){
    custom_container * buffers[threads];
    int buffSize = 200;
    for (int i = 0; i < threads; i++){
        buffers[i] = new custom_container(partitions, buffSize, container1);
    }
    #pragma omp parallel for num_threads(threads)
    for (int i = 0; i < n1; i += 1){
        int p1 = c1[i] % partitions;
        buffers[omp_get_thread_num()]->push_back(p1, c1[i]);
        buffers[omp_get_thread_num()]->manageWriteBuff(p1);
    }
    for(int i = 0;i < threads; i++){
        buffers[i]->flushAll();
        buffers[i]->mainC = container2;
    }
    #pragma omp parallel for num_threads(threads)
    for (int i = 0; i < n2; i += 1){
        int p1 = c2[i] % partitions;
        buffers[omp_get_thread_num()]->push_back(p1, c2[i]);
        buffers[omp_get_thread_num()]->manageWriteBuff(p1);
    }
    for(int i = 0;i < threads; i++){
        buffers[i]->flushAll();
    }
    for(int i = 0;i < threads; i++){
        free(buffers[i]);
    }
}

void probeSIMDSerial(custom_container & container1, custom_container & container2, int pSize1, int partitions){
    int buffSize = 256 * 128 / partitions;
    int count = 0;
    int maxBucketSize = 0;
    for(int p = 0; p < container1.partition; p++){
        maxBucketSize = max(maxBucketSize, container1.getPartitionSize(p));
    }
    maxBucketSize = maxBucketSize / partitions;
    custom_container * hashContainer = new custom_container(partitions, maxBucketSize, (custom_container * ) NULL);
    for(int p = 0; p < container1.partition; p++){
        hashContainer->reset();
        int * col1 = container1.getPartition(p);
        int * col2 = container2.getPartition(p);
        int tail1 = container1.getPartitionSize(p);
        int tail2 = container2.getPartitionSize(p);
        
        //build hash table
        int hashVal;
        for(int i = 0; i < tail1; i++){
            hashVal = col1[i] % partitions;
            hashContainer->push_back(hashVal, col1[i]);
        }
        //SIMD-based probe
        for(int i = 0; i < tail2; i++){
            hashVal = col2[i] % partitions;
            int * bucket = hashContainer->getPartition(hashVal);
            __m256i search_key = _mm256_set1_epi32(col2[i]);
            int j = 0;
            int bSize = hashContainer->getPartitionSize(hashVal);
            int exists;
            for(;j <  bSize - 8; j += 8){
                __m256i values = _mm256_loadu_si256((__m256i *)(bucket + j));
                values = _mm256_cmpeq_epi32(values, search_key);
                exists = _mm256_movemask_epi8(values);
                if (exists != 0){
                    count++;
                }
            }
            for(; j < bSize; j++){
                if(bucket[j] == col2[i]){
                    count++;
                }
            }
        }
    }
    cout << "Matched - " << count <<  endl;
}


void probeSIMDParallel(custom_container & container1, custom_container & container2, int pSize1, int partitions){
    int buffSize = 256 * 128 / partitions;
    int maxBucketSize = 0;
    for(int p = 0; p < container1.partition; p++){
        maxBucketSize = max(maxBucketSize, container1.getPartitionSize(p));
    }
    maxBucketSize = maxBucketSize / partitions;
    custom_container * hashContainers[threads];
    for(int i = 0; i < threads; i++){
        hashContainers[i] = new custom_container(partitions, maxBucketSize, (custom_container * ) NULL);    
    }

    int count = 0;
    #pragma omp parallel for num_threads(threads) reduction(+: count)
    for(int p = 0; p < container1.partition; p++){
        custom_container * hashContainer = hashContainers[omp_get_thread_num()];
        hashContainer->reset();
        int * col1 = container1.getPartition(p);
        int * col2 = container2.getPartition(p);
        int tail1 = container1.getPartitionSize(p);
        int tail2 = container2.getPartitionSize(p);
        
        //build hash table
        int hashVal;
        for(int i = 0; i < tail1; i++){
            hashVal = col1[i] % partitions;
            hashContainer->push_back(hashVal, col1[i]);
        }
        //SIMD-based probe
        for(int i = 0; i < tail2; i++){
            hashVal = col2[i] % partitions;
            int * bucket = hashContainer->getPartition(hashVal);
            __m256i search_key = _mm256_set1_epi32(col2[i]);
            int j = 0;
            int bSize = hashContainer->getPartitionSize(hashVal);
            int exists;
            for(;j <  bSize - 8; j += 8){
                __m256i values = _mm256_loadu_si256((__m256i *)(bucket + j));
                values = _mm256_cmpeq_epi32(values, search_key);
                exists = _mm256_movemask_epi8(values);
                if (exists != 0){
                    count++;
                }
            }
            for(; j < bSize; j++){
                if(bucket[j] == col2[i]){
                    count++;
                }
            }
        }
    }
    cout << "Matched: " << count <<  endl;
}


void partitionedHash(int * c1, int * c2, int n1, int n2, int partitions, bool serial, bool simd){
    int pSize1 = (n1/partitions);
    int pSize2 = (n2/partitions);
    int bufSize = (128*256*3/partitions);
    custom_container container1(partitions, pSize1, (custom_container *)NULL);
    custom_container container2(partitions, pSize2, (custom_container *)NULL);
    custom_container buff1(partitions, bufSize, &container1);
    custom_container buff2(partitions, bufSize, &container2);
    //partition the elements
    auto start = std::chrono::high_resolution_clock::now();
    if (serial){
        hashPartition(c1,c2,n1,n2,partitions,buff1,buff2);
    } else {
        hashPartitionParallel(c1,c2,n1,n2,partitions,&container1,&container2);
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;
    std::cout << "Partition Time: " << elapsed.count() << " seconds" << std::endl;
    //unordered_set<int> exists(pSize1);
    start = std::chrono::high_resolution_clock::now();
    if (serial){
        //cout << "Serial" << endl;
        if(simd)
            probeSIMDSerial(container1, container2, pSize1, 511);    
        else
            probeTableSerial(container1, container2, pSize1, partitions);
    } else {
        if (simd)
            probeSIMDParallel(container1, container2, pSize1, 1013);
        else
            probeTableParallel(container1, container2, pSize1, partitions);    
    }
    end = std::chrono::high_resolution_clock::now();
    elapsed = end - start;
    std::cout << "Probe Time: " << elapsed.count() << " seconds" << std::endl;
}