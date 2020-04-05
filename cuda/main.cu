#include <iostream>
#include <fstream>
#include <thrust/host_vector.h>
#include <thrust/device_vector.h>
#include <thrust/generate.h>
#include <thrust/sort.h>
#include <thrust/copy.h>
#include <vector>
#include <unordered_set>
#include <algorithm>
#include <string>
#include <unordered_map>
#include <string>
#include <functional>
#include <math.h>
#include <emmintrin.h>
#include <string.h>
#include<chrono>
#include <omp.h>
#include "general.cuh"
#include "hash_approach.cuh"
#include "sort_approach.cuh"
using namespace std;
int pNo = 127;

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
    int * dCol1 = 0;
    int * dCol2 = 0;
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
        //partitionedSortSerial(col1, col2, c1, c2, pNo);
    }
    else if (strcmp(funcName, "partitionedSortParallel") == 0){
        partitionedSortParallel(col1, col2, c1, c2, pNo);
    } else if(strcmp(funcName, "sortParallel") == 0){
        sortParallel(col1, col2, c1, c2, pNo);
    }
    else if (strcmp(funcName, "basicSort") == 0){
        basicSort(col1, col2, c1, c2);
    } else if (strcmp(funcName, "partitionedHashSerial") == 0) {
      //partitionedHash(col1, col2, c1, c2, pNo, true);  
    } else if (strcmp(funcName, "partitionedHashParallel") == 0) {
        partitionedHash(col1, col2, c1, c2, pNo, false);  
    }
    else if (strcmp(funcName, "basicHash") == 0) {
      //basicHash(col1, col2, c1, c2);  
    } else {
        cout << "Invalid function name" << endl;
    }
    //cudaThreadSynchronize();
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;
    std::cout << "Total Time: " << elapsed.count() << " ms" << std::endl;
    delete[] col1;
    delete[] col2;
    return 0;
}