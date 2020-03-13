#include<iostream>
#include<algorithm>
#include<omp.h>
#include<chrono>
#include "general.h"
using namespace std;
void basicSort(int * col1, int * col2, int c1, int c2){
    int cnt = 0;
    sort(col1, col1 + c1);
    sort(col2, col2 + c2);
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

void partition(int * c1, int * c2, int n1, int n2, int partitons, custom_container & buff1, custom_container & buff2){
    int p1;
    int p2;
    int p3;
    int p4;
    for (int i = 0; i < n1; i += 1){
        p1 = c1[i] % partitons;
        buff1.push_back(p1, c1[i]);
        buff1.manageWriteBuff(p1);
    }
    buff1.flushAll();
    for (int i = 0; i < n2; i += 1){
        p1 = c2[i] % partitons;
        buff2.push_back(p1, c2[i]);
        buff2.manageWriteBuff(p1);
    }
    buff2.flushAll();
}

void partitionedSortSerial(int * c1, int * c2, int n1, int n2, int partitions){
    int pSize1 = (2) * (n1/partitions);
    int pSize2 = (2) * (n2/partitions);
    int bufSize = (128*256*3/partitions);
    custom_container container1(partitions, pSize1, (custom_container *)NULL);
    custom_container container2(partitions, pSize2, (custom_container *)NULL);
    custom_container buff1(partitions, bufSize, &container1);
    custom_container buff2(partitions, bufSize, &container2);
    //partition the elements
    auto start = std::chrono::high_resolution_clock::now();
    partition(c1,c2,n1,n2,partitions,buff1,buff2);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;
    std::cout << "Partition Time: " << elapsed.count() << " seconds" << std::endl;
    int match = 0;
    start = std::chrono::high_resolution_clock::now();
    for (int p = 0; p < partitions; p++){
        int * col1 = container1.getPartition(p);
        int * col2 = container2.getPartition(p);
        int head1 = container1.getPartitionSize(p);
        int head2 = container2.getPartitionSize(p);
        sort(col1, col1 + head1);
        sort(col2, col2 + head2);
        int i = 0; int j = 0;
        while (i < head1 && j < head2){
            if (col1[i] < col2[j]){
                i++;
            } else if (col1[i] > col2[j]){
                j++;
            } else{
                match++;
                j++;
                i++;
            }
        }
    }
    end = std::chrono::high_resolution_clock::now();
    elapsed = end - start;
    std::cout << "Sort + merge Time: " << elapsed.count() << " seconds" << std::endl;
    cout << "Matched: " << match <<endl;
}

void partitionParallel(int * c1, int * c2, int n1, int n2, int partitions, custom_container * container1, custom_container * container2){
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


void partitionedSortParallel(int * c1, int * c2, int n1, int n2, int partitions){
    int pSize1 = (5) * (n1/(partitions));
    int pSize2 = (5) * (n2/(partitions));
    int bufSize = (128*256/partitions);
    custom_container container1(partitions, pSize1, (custom_container *)NULL);
    custom_container container2(partitions, pSize2, (custom_container *)NULL);
    //custom_container buff1(partitions, bufSize, &container1);
    //custom_container buff2(partitions, bufSize, &container2);
    //partition the elements
    auto start = std::chrono::high_resolution_clock::now();
    partitionParallel(c1,c2,n1,n2,partitions,&container1,&container2);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;
    std::cout << "Partition Time: " << elapsed.count() << " seconds" << std::endl;
    int match = 0;
    start = std::chrono::high_resolution_clock::now();
    #pragma omp parallel for num_threads(threads) reduction(+: match)
    for (int p = 0; p < partitions; p++){
        int * col1 = container1.getPartition(p);
        int * col2 = container2.getPartition(p);
        int head1 = container1.getPartitionSize(p);
        int head2 = container2.getPartitionSize(p);
        sort(col1, col1 + head1);
        sort(col2, col2 + head2);
        int i = 0; int j = 0;
        while (i < head1 && j < head2){
            if (col1[i] < col2[j]){
                i++;
            } else if (col1[i] > col2[j]){
                j++;
            } else{
                match++;
                j++;
                i++;
            }
        }
    }
    end = std::chrono::high_resolution_clock::now();
    elapsed = end - start;
    std::cout << "Sort + Merge Time: " << elapsed.count() << " seconds" << std::endl;
    cout << "Matched: " << match <<endl;
}