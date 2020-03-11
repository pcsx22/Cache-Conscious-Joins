#include<iostream>
#include<algorithm>
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
    int bufSize = (128*256/partitions);
    custom_container container1(partitions, pSize1, (custom_container *)NULL);
    custom_container container2(partitions, pSize2, (custom_container *)NULL);
    custom_container buff1(partitions, bufSize, &container1);
    custom_container buff2(partitions, bufSize, &container2);
    //partition the elements
    partition(c1,c2,n1,n2,partitions,buff1,buff2);
    int match = 0;
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
    cout << "Matched: " << match <<endl;
}

void partitionedSortParallel(int * c1, int * c2, int n1, int n2, int partitions){
    int pSize1 = (10) * (n1/partitions);
    int pSize2 = (10) * (n2/partitions);
    int bufSize = (128*256/partitions);
    custom_container container1(partitions, pSize1, (custom_container *)NULL);
    custom_container container2(partitions, pSize2, (custom_container *)NULL);
    custom_container buff1(partitions, bufSize, &container1);
    custom_container buff2(partitions, bufSize, &container2);
    //partition the elements
    partition(c1,c2,n1,n2,partitions,buff1,buff2);
    int match = 0;
    #pragma omp parallel for num_threads(8) reduction(+: match)
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
    cout << "Matched: " << match <<endl;
}