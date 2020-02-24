#include <unordered_set>
#include<algorithm>
#include<iostream>
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
    for (int i = 0; i < n2; i++){
        p = c2[i] % partitons;
        buff2.push_back(p, c2[i]);
        buff2.manageWriteBuff(p);
    }
}

void partionedHash(int * c1, int * c2, int n1, int n2, int partitions){
    int pSize1 = (2) * (n1/partitions);
    int pSize2 = (2) * (n2/partitions);
    int bufSize = (110*256/partitions);
    custom_container container1(partitions, pSize1, (custom_container *)NULL);
    custom_container container2(partitions, pSize2, (custom_container *)NULL);
    custom_container buff1(partitions, bufSize, &container1);
    custom_container buff2(partitions, bufSize, &container2);
    //partition the elements
    hashPartition(c1,c2,n1,n2,partitions,buff1,buff2);
    unordered_set<int> exists(pSize1);
    int cnt = 0;
    for(int i = 0; i < partitions; i++){
        int * col1 = container1.getPartition(i);
        int * col2 = container2.getPartition(i);
        int s1 = container1.getPartitionSize(i);
        int s2 = container2.getPartitionSize(i);
        //build hash table for col1
        
        for (int j = 0; j < s1; j++){
            exists.insert(col1[j]);
        }
        //probe for col2
        for (int j = 0; j < s2; j++){
            if (exists.find(col2[j]) != exists.end()){
                cnt++;
            }
        }
        exists.clear();
        exists.reserve(s1);
    }
        cout << "Matched: " << cnt << endl;
}