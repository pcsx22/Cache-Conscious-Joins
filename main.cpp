#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_set>
#include <algorithm>
#include <string>
#include <bitset>
#include <unordered_map>
#include <string>
#include <math.h>
#include <emmintrin.h>
#include <string.h>
#include "general.h"
#include "hash_approach.h"
using namespace std;
int pNo = 127;
//TODO: Add Loop Unrolling to introduce instruction level parallelism

void bitSetAlgo(int * arr1, int * arr2, int c1, int c2){
    bitset<1000000000> * bSet = new bitset<1000000000>();
    for (int i = 0; i<c1 && i < c2; i++){
        bSet->set(arr1[i]);
    }
    int cnt = 0;
    for (int i = 0;i < c2; i++){
        
        if (bSet->test(arr2[i])){
            cnt += 1;
        }
    }
    cout << cnt << endl;
}

void loopOverBlock(int * arr1, int * arr2, int & c1, int & c2, int blkSize){
    int cnt = 0;
    int N = c1 / blkSize;
    for (int blkNo = 0; blkNo < N; blkNo++){
        for (int i = 0; i < c1; i++){
            for (int j = blkNo * blkSize; j < (blkNo + 1) * blkSize; j++){
                if (arr1[i] == arr2[j]){
                    cnt += 1;
                }
            }
        }
    }
    cout << cnt << endl;
}

void basicLoop(int * arr1, int * arr2, int & c1, int & c2){
    int cnt = 0;
        for (int i = 0; i < c1; i++){
            for (int j = 0; j < c2; j++){
                if (arr1[i] == arr2[j]){
                    cnt += 1;
                }
            }
        }
    cout << cnt << endl;
}
// void partition(int * c1, int * c2, int n1, int n2, unordered_map<int, vector<int> > &p1, unordered_map<int, vector<int> > &p2, int vecSize) {
//     void * random =  get_random_key_for_clhash(UINT64_C(0x23a23cf5033c3c81),UINT64_C(0xb3816f6a2c68e530));
//     char int_buffer[11];
//     int hashValue1;
//     int hashValue2;
//     int hashValue3;
//     int hashValue4;
//     for (int i = 0; i < min(n1, n1-4); i += 4){
//         hashValue1 = clhash(random, to_string(c1[i]).c_str(), hashSize);
//         hashValue2 = clhash(random, to_string(c1[i+1]).c_str(), hashSize);
//         hashValue3 = clhash(random, to_string(c1[i+2]).c_str(), hashSize);
//         hashValue4 = clhash(random, to_string(c1[i+3]).c_str(), hashSize);
//         if (p1.find(hashValue1) == p1.end()){
//             p1[hashValue1].reserve(vecSize);
//         }
//         p1[hashValue1].push_back(c1[i]);
//         if (p1.find(hashValue2) == p1.end()){
//             p1[hashValue2].reserve(vecSize);
//         }
//         p1[hashValue2].push_back(c1[i+1]);
//         if (p1.find(hashValue3) == p1.end()){
//             p1[hashValue3].reserve(vecSize);
//         }
//         p1[hashValue3].push_back(c1[i+2]);
//         if (p1.find(hashValue4) == p1.end()){
//             p1[hashValue4].reserve(vecSize);
//         }
//         p1[hashValue4].push_back(c1[i+3]);
//     }
//     for (int i = 0; i < min(n2, n2-4); i += 4){
//         hashValue1 = clhash(random, to_string(c2[i]).c_str(), hashSize);
//         hashValue2 = clhash(random, to_string(c2[i+1]).c_str(), hashSize);
//         hashValue3 = clhash(random, to_string(c2[i+2]).c_str(), hashSize);
//         hashValue4 = clhash(random, to_string(c2[i+3]).c_str(), hashSize);
//         if (p2.find(hashValue1) == p2.end()){
//             p2[hashValue1].reserve(vecSize);
//         }
//         p2[hashValue1].push_back(c2[i]);
//         if (p2.find(hashValue2) == p2.end()){
//             p2[hashValue2].reserve(vecSize);
//         }
//         p2[hashValue2].push_back(c2[i+1]);
//         if (p2.find(hashValue3) == p2.end()){
//             p2[hashValue3].reserve(vecSize);
//         }
//         p2[hashValue3].push_back(c2[i+2]);
//         if (p2.find(hashValue4) == p2.end()){
//             p2[hashValue4].reserve(vecSize);
//         }
//         p2[hashValue4].push_back(c2[i+3]);
//     }
// }
// void partition1(int * c1, int * c2, int n1, int n2, unordered_map<int, vector<int> > &p1, unordered_map<int, vector<int> > &p2, int vecSize) {
//     int partitions = 9 * (int) pow(10,hashSize - 1);
//     int bufSize = (int) (17000/partitions);
//     custom_container buffers(partitions + 10, bufSize, &p1);
//     custom_container buffers1(partitions + 10, bufSize, &p2);
//     unordered_map<int, int> bufMap(partitions + 10);
//     void * random =  get_random_key_for_clhash(UINT64_C(0x23a23cf5033c3c81),UINT64_C(0xb3816f6a2c68e530));
//     char int_buffer[11];
//     int hashValue;
//     int index = 0;
//     for (int i = 0; i < n1; i++){
//         hashValue = clhash(random, static_cast<char*>(static_cast<void*>(&c1[i])), hashSize);
//         if (p1.find(hashValue) == p1.end()){
//             p1[hashValue].reserve(vecSize);
//             bufMap[hashValue] =  index++;
//         }
//         buffers.push_back(bufMap[hashValue], c1[i]);
//         buffers.manageWrite(bufMap[hashValue], hashValue);
//     }
//     index = 0;
//     bufMap.clear();
//     for (int i = 0; i < n2; i++){
//         hashValue = clhash(random, to_string(c2[i]).c_str(), hashSize);
//         if (p2.find(hashValue) == p2.end()){
//             p2[hashValue].reserve(vecSize);
//             bufMap[hashValue] =  index++;
//         }
//         buffers1.push_back(bufMap[hashValue], c2[i]);
//         buffers1.manageWrite(bufMap[hashValue], hashValue);
//     }
// }

// void partitionSort(int * c1, int * c2, int n1, int n2, int & partitionSize){
//     unordered_map<int, vector<int> > p1(9*(int) pow(10, hashSize-1) + 100);
//     unordered_map<int, vector<int> > p2(9*(int) pow(10, hashSize-1) + 100);
//     //partition the elements
//     partition(c1,c2,n1,n2,p1,p2, (n1/ (9 * (int)pow(10,hashSize-1))) + (n1/(1 * (int)pow(10,hashSize))));
//     unordered_map<int, vector<int> >::iterator it;
//     int match = 0;
//     for (it = p1.begin(); it != p1.end(); it++){
//         if (p2.find(it->first) != p2.end()){
//             vector<int> col1 = p1[it->first];
//             vector<int> col2 = p2[it->first];
//             sort(col1.begin(), col1.end());
//             sort(col2.begin(), col2.end());
//             int i = 0; int j = 0;
//             while (i < col1.size() && j < col2.size()){
//                 if (col1.at(i) < col2.at(j)){
//                     i++;
//                 } else if (col1.at(i) > col2.at(j)){
//                     j++;
//                 } else{
//                     match++;
//                     j++;
//                     i++;
//                 }
//             }
//         }
//     }
//     cout << "Matched: " << match <<endl;
// }

// void partition2(int * c1, int * c2, int n1, int n2, custom_container& buffers1,custom_container& buffers2) {
//     int partitions = 9 * (int) pow(10,hashSize - 1) + 10;
//     int bufSize = (int) (150000/partitions);
//     custom_container buff1(partitions, bufSize, &buffers1);
//     custom_container buff2(partitions, bufSize, &buffers2);
//     unordered_map<int, int> bufMap(partitions + 10);
//     void * random =  get_random_key_for_clhash(UINT64_C(0x23a23cf5033c3c81),UINT64_C(0xb3816f6a2c68e530));
//     char int_buffer[11];
//     int hashValue;
//     int index = 0;
//     for (int i = 0; i < n1; i++){
//         hashValue = clhash(random, static_cast<char*>(static_cast<void*>(&c1[i])), hashSize);
//         if (bufMap.find(hashValue) == bufMap.end()){
//             bufMap[hashValue] =  index++;
//         }
//         buff1.push_back(bufMap[hashValue], c1[i]);
//         buff1.manageWriteBuff(bufMap[hashValue]);
//         //buffers.manageWrite(bufMap[hashValue], hashValue);
//     }
//     for (int i = 0; i < n2; i++){
//         hashValue = clhash(random, static_cast<char*>(static_cast<void*>(&c2[i])), hashSize);
//         if (bufMap.find(hashValue) == bufMap.end()){
//             continue;
//         }
//         buff2.push_back(bufMap[hashValue], c2[i]);
//         buff2.manageWriteBuff(bufMap[hashValue]);
//         //buffers1.manageWrite(bufMap[hashValue], hashValue);
//     }
// }

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


void partitionSort2(int * c1, int * c2, int n1, int n2, int partitions) {
    int pSize1 = (10) * (n1/partitions);
    int pSize2 = (10) * (n2/partitions);
    int bufSize = (150*256/partitions);
    custom_container container1(partitions, pSize1, (custom_container *)NULL);
    custom_container container2(partitions, pSize2, (custom_container *)NULL);
    custom_container buff1(partitions, bufSize, &container1);
    custom_container buff2(partitions, bufSize, &container2);
    //partition the elements
    partition(c1,c2,n1,n2,pNo,buff1,buff2);

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

int main(int argc, char ** argv){
    if (argc < 6){
        cout << "Wrong number of argument. Use ./join num_of_elements func_name fileA fileB operator";
        return 0;
    }
    int cacheSize = 12000;
    int partitionSize = 1200;
    int inputSize = stoi(argv[1]) * 1000;
    int * col1 = new int[inputSize];
    int * col2 = new int[inputSize];
    char * funcName = argv[2];
    char * file1 = argv[3];
    char * file2 = argv[4];
    char * condition = argv[5];
    if (argc == 7){
        pNo = stoi(argv[6]);
    }
    ifstream reader;
    reader.open(file1);
    int x;
    int c1 = 0;
    while (c1 < inputSize && reader >> col1[c1++]){
    }
    ifstream reader1;
    reader1.open(file2);
    int c2 = 0;
    while (c2 < inputSize && reader1 >> col2[c2++]){
    }
    reader.close();
    reader1.close();
    if (strcmp(funcName, "basicNestedLoop") == 0){
        basicLoop(col1, col2, c1, c2);
    } else if (strcmp(funcName, "blockedNestedLoop") == 0){
        loopOverBlock(col1, col2, c1, c2, cacheSize);
    } else if (strcmp(funcName, "partitionedSort") == 0){
        partitionSort2(col1, col2, c1, c2, pNo);    
    } else if (strcmp(funcName, "basicSort") == 0){
        basicSort(col1, col2, c1, c2);
    } else if (strcmp(funcName, "partitionedHash") == 0) {
      partionedHash(col1, col2, c1, c2, pNo);  
    } else if (strcmp(funcName, "basicHash") == 0) {
      basicHash(col1, col2, c1, c2);  
    } else {
        cout << "Invalid function name" << endl;
    }
    free(col1);
    free(col2);
    return 0;
}