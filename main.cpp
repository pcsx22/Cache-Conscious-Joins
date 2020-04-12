#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_set>
#include <algorithm>
#include <string>
#include <bitset>
#include <unordered_map>
#include <string>
#include <functional>
#include <math.h>
#include <emmintrin.h>
#include <string.h>
#include<chrono>
#include <omp.h>
#include "general.h"
#include "hash_approach.h"
#include "sort_approach.h"
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

void readFile(char * file1, char * file2, int * col1, int * col2, int *c1, int *c2, int inputSize){
    char * file;
    int * container;
    ifstream reader1(file1);
    ifstream reader2(file2);
    for (int f = 0; f < 2; f++){
        int * c;
        ifstream * reader;
        if (f == 0){
            file = file1;
            container = col1;
            c = c1;
            reader = &reader1;
        } else {
            file = file2;
            container = col2;
            c = c2;
            reader = &reader2;
        }
        
        int lc = 0;
        int item = 0;
        char buf[2048];
        while(!reader->eof() && *c < inputSize)
        {
            reader->read(buf, sizeof(buf));
            uint64_t k = reader->gcount();
            for (uint64_t i = 0; i < k && *c < inputSize; ++i)
            {
                switch (buf[i])
                {
                    case '\r':
                        break;
                    case '\n':
                        item = 0;
                        break;
                    case ' ':
                        container[*c] = item;
                        *c = *c + 1;
                        item = 0;
                        break;
                    case '0': case '1': case '2': case '3':
                    case '4': case '5': case '6': case '7':
                    case '8': case '9':
                        item = 10*item + buf[i] - '0';
                        break;
                    default:
                        std::cerr << "Bad format\n";
                }    
            }
        }
            __builtin_prefetch(container + *c + 32, 1, 0);
            reader->close();
            reader->clear();
    }
}

int threads = 1;
int main(int argc, char ** argv){
    if (argc < 7){
        cout << "Wrong number of argument. Use ./join num_of_elements func_name fileA fileB operator";
        return 0;
    }
    int cacheSize = 12000;
    int partitionSize = 1200;
    int inputSize = pow(10, stoi(argv[1]));
    int * col1 = new int[inputSize + inputSize/100];
    int * col2 = new int[inputSize + inputSize/100];
    char * funcName = argv[2];
    char * file1 = argv[3];
    char * file2 = argv[4];
    char * condition = argv[5];
    int threadNum = stoi(argv[6]);
    threads = threadNum;
    if (argc == 8){
        pNo = stoi(argv[7]);
    }
    int c1 = 0;
    int c2 = 0;
    readFile(file1, file2, col1, col2, &c1, &c2, inputSize);
    auto start = std::chrono::high_resolution_clock::now();
    if (strcmp(funcName, "basicNestedLoop") == 0){
        basicLoop(col1, col2, c1, c2);
    } else if (strcmp(funcName, "blockedNestedLoop") == 0){
        loopOverBlock(col1, col2, c1, c2, cacheSize);
    } else if (strcmp(funcName, "partitionedSortSerial") == 0){
        partitionedSortSerial(col1, col2, c1, c2, pNo);
    } else if (strcmp(funcName, "partitionedSortSerialSIMD") == 0){
        partitionedSortSerialSIMD(col1, col2, c1, c2, pNo);
    }
    else if (strcmp(funcName, "partitionedSortParallel") == 0){
        partitionedSortParallel(col1, col2, c1, c2, pNo);
    } else if (strcmp(funcName, "partitionedSortParallelSIMD") == 0){
        partitionedSortParallelSIMD(col1, col2, c1, c2, pNo);
    }
    else if (strcmp(funcName, "basicSort") == 0){
        basicSort(col1, col2, c1, c2);
    } else if (strcmp(funcName, "partitionedHashSerial") == 0) {
      partitionedHash(col1, col2, c1, c2, pNo, true, false);  
    } else if (strcmp(funcName, "partitionedHashParallel") == 0) {
      partitionedHash(col1, col2, c1, c2, pNo, false, false);  
    } else if (strcmp(funcName, "partitionedHashSerialSIMD") == 0) {
      partitionedHash(col1, col2, c1, c2, pNo, true, true);  
    }
    else if (strcmp(funcName, "partitionedHashParallelSIMD") == 0) {
      partitionedHash(col1, col2, c1, c2, pNo, false, true);  
    }
    else if (strcmp(funcName, "basicHash") == 0) {
      basicHash(col1, col2, c1, c2);  
    } else {
        cout << "Invalid function name" << endl;
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;
    std::cout << "Total Time: " << elapsed.count() << " ms" << std::endl;
    delete[] col1;
    delete[] col2;
    return 0;
}