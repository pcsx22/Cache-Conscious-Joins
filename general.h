#include <unordered_map>
#include <vector>
#include <iostream>
#include<omp.h>
#include<cstring>
#include <emmintrin.h>
#include <smmintrin.h>  
#include <x86intrin.h>
#include <stdlib.h>
using namespace std;
extern int threads;
#ifndef NEXT_POW_2
/** 
 *  compute the next number, greater than or equal to 32-bit unsigned v.
 *  taken from "bit twiddling hacks":
 *  http://graphics.stanford.edu/~seander/bithacks.html
 */
#define NEXT_POW_2(V)                           \
    do {                                        \
        V--;                                    \
        V |= V >> 1;                            \
        V |= V >> 2;                            \
        V |= V >> 4;                            \
        V |= V >> 8;                            \
        V |= V >> 16;                           \
        V++;                                    \
    } while(0)
#endif

class custom_container {
    public:
    int * container;
    int * head;
    custom_container * mainC;
    unordered_map<int, vector<int> > * p;
    int bufSize;
    int partition;
    custom_container(){
        this->mainC = NULL;
        this->container = NULL;
        this->head = NULL;
    }
    
    custom_container(int partition, int bufSize, custom_container * c){
        this->bufSize = bufSize;
        if (c == NULL)
            NEXT_POW_2(this->bufSize);
        this->partition = partition;
        if(posix_memalign((void **)&this->container, 64, partition * this->bufSize * sizeof(int)) != 0)
            throw std::bad_alloc();
        this->head = new int[partition];
        for (int i = 0;i < partition; i++){
            this->head[i] = 0;
        }
        this->mainC = c;
    }

    void push_back(int partition, int el){
        //int j = __sync_fetch_and_add(this->head + partition, 1);
        this->container[partition * this->bufSize + this->head[partition]] = el;
        this->head[partition] += 1;
    }

    void insert(int partition, int * els, int n){
        int * p = this->getPartition(partition);
        //int& h = this->head[partition];
        int j = __atomic_add_fetch(this->head + partition, n, __ATOMIC_ACQUIRE);
        int h = j - n;
        p = p + h;
        int i = 0;
        for(;i < n; i += 1){
            //p[h++] = els[i];
            //__m128i * d1 = (__m128i*) p;
            //__m128i s2 = _mm_loadu_si128((__m128i *) (els + i));
            //_mm_stream_si128(d1, s2);
            _mm_stream_si32(p, els[i]);
            p += 1;
        }
        // for(;i<n;i++){
        //     _mm_stream_si32(p, els[i]);
        //     p += 1;
        // }
    }
    

    void flush(int p){
        int * pAddr = this->container + p * this->bufSize;
        if (this->head[p] > 0)
            this->mainC->insert(p, pAddr, this->head[p]);
        this->head[p] = 0;
    }
    
    void manageWriteBuff(int partition){
        if (this->head[partition] == this->bufSize){
            this->flush(partition);
        }
    }

    void flushAll(){
        for(int i = 0;i < this->partition; i++){
            this->flush(i);
        }
    }

    void manageWrite(int partition, int hashValue){
        if (this->head[partition] == this->bufSize){
            int * pAddr = this->container + partition * this->bufSize;
            auto& m = *(this->p);
            m[hashValue].insert(m[hashValue].end(), pAddr, pAddr + this->bufSize);
            this->head[partition] = 0;
        }
    }

    int* getPartition(int partition){
        return this->container + partition * this->bufSize;
    }

    int getPartitionSize(int partition){
        return this->head[partition];
    }

    void reset(){
        memset(this->head, 0, sizeof(int) * this->partition);
    }

    ~custom_container(){
        free(this->container);
        free(this->head);
    }
};

#define HASH_BIT_MODULO(K, MASK, NBITS) (((K) & MASK) >> NBITS)

inline 
uint32_t 
get_hist_size(uint32_t relSize) 
{
    NEXT_POW_2(relSize);
    relSize >>= 2;
    if(relSize < 4) relSize = 4; 
    return relSize;
}


class CustomHashTable{
    public:
    int * container;
    int * next;
    int size;
    uint32_t nSize;
    uint32_t mask;
    CustomHashTable(int n){
        this->size = n;
        this->nSize = get_hist_size(n);
        this->container = new int[nSize];
        this->next = new int[n];
        this->mask = (nSize-1) << 18;
    }

    void insert(int el){
        uint32_t idx = HASH_BIT_MODULO(el, this->mask, 18);
        if (this->container[idx] != 0){
            while (this->container[idx] != 0){
                idx++;
            }
        }
        this->container[idx] = el;
    }

    int find(int el){
        int idx = HASH_BIT_MODULO(el, this->mask, 18);
        if (this->container[idx] == 0){
            return -1;
        }
        while (this->container[idx] != el and idx < this->size){
            idx++;
        }
        if (idx == this->size){
            return -1;
        } else {
            return idx;
        }
    }

    int end(){
        return -1;
    }

    void flush(){
        for (int i = 0; i < this->size; i++){
            this->container[i] = 0;
        }
    }

    ~CustomHashTable(){
        free(this->container);
    }
};


inline bool processEquality(int a, int b){
    return a == b;
}

inline bool processGreaterThan(int a, int b){
    return a > b;
}

inline bool processLessThan(int a, int b){
    return a < b;
}