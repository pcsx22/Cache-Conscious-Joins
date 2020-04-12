#include "simd_sort.h"
#include<vector>
#include<algorithm>
#include <cstdlib>
#include<iostream>
#include<chrono>
#include<string.h>
#include <random>
int main(int argc, char ** argv){
        int N = 1 << std::stoi(argv[1]);
        std::vector<int> h_vec(N);
        int * data;
        aligned_init<int>(data, N);
        std::random_device rd;  //Will be used to obtain a seed for the random number engine
        std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
        std::uniform_int_distribution<int> dis(0, N);
        for(size_t i = 0; i < N; i++) {
                data[i] = dis(gen);
        }

        auto start = std::chrono::high_resolution_clock::now();
        if (argc > 2){
                avx2::SIMDSort(N, data);
        }
        else
        {
                std::sort(data, data + N);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = end - start;
        std::cout << "Total Time: " << elapsed.count() << " -ms" << std::endl;
        std::cout << data[1] << std::endl;
        return 0;
}
