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
    int pSize1 = (10)*(n1/partitons);
    int pSize2 = (10)*(n2/partitons);
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
    int threadNums = 8;
    unordered_set <int> exists(pSize);
    // exists.set_empty_key(NULL);
    // exists.set_deleted_key(-1);
    int cnt = 0;
    for(int i = 0; i < partitions; i++){
        int * col1 = buff1.getPartition(i);
        int s1 = buff1.getPartitionSize(i);
        //build hash table for col1
        int tId = omp_get_thread_num();
        for (int j = 0; j < s1; j++){
            exists.insert(col1[j]);
        }

        int * col2 = buff2.getPartition(i);
        int s2 = buff2.getPartitionSize(i);
        //probe for col2
        for (int j = 0; j < s2; j++){
            if (exists.find(col2[j]) != exists.end()){
                cnt++;
//                exists.erase(col2[j]);
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
    int threadNums = 8;
    vector<unordered_set<int>* >hList(threadNums);
    for(int i = 0; i < threadNums; i++){
        hList[i] = new unordered_set<int>(pSize);
        // hList[i]->set_empty_key(NULL);
        // hList[i]->set_deleted_key(-1);    
    }
    int cnt = 0;
    #pragma omp parallel for num_threads(threadNums) reduction(+: cnt)
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

void partitionedHash(int * c1, int * c2, int n1, int n2, int partitions, bool serial){
    int pSize1 = (2) * (n1/partitions);
    int pSize2 = (2) * (n2/partitions);
    int bufSize = (110*256/partitions);
    custom_container container1(partitions, pSize1, (custom_container *)NULL);
    custom_container container2(partitions, pSize2, (custom_container *)NULL);
    custom_container buff1(partitions, bufSize, &container1);
    custom_container buff2(partitions, bufSize, &container2);
    //partition the elements
    auto start = std::chrono::high_resolution_clock::now();
    hashPartition(c1,c2,n1,n2,partitions,buff1,buff2);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;
    std::cout << "Partition Time: " << elapsed.count() << " seconds" << std::endl;
    //unordered_set<int> exists(pSize1);
    start = std::chrono::high_resolution_clock::now();
    if (serial){
        probeTableParallel(container1, container2, pSize1, partitions);    
    } else {
        probeTableParallel(container1, container2, pSize1, partitions);    
    }
    end = std::chrono::high_resolution_clock::now();
    elapsed = end - start;
    std::cout << "Probe Time: " << elapsed.count() << " seconds" << std::endl;
}

void probeBucketTable(custom_container &buff1, custom_container &buff2, int pSize, int partitions){
    
}