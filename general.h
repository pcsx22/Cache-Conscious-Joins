#include <unordered_map>
#include <vector>
#include <iostream>
#include <emmintrin.h>
using namespace std;
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
    custom_container(int partition, int bufSize, unordered_map<int, vector<int> > * p){
        this->bufSize = bufSize;
        this->partition = partition;
        this->container = new int[partition * bufSize];
        this->head = new int[partition];
        for (int i = 0;i < partition; i++){
            this->head[i] = 0;
        }
        this->p = p;
    }
    custom_container(int partition, int bufSize, custom_container * c){
        this->bufSize = bufSize;
        this->partition = partition;
        this->container = new int[partition * bufSize];
        this->head = new int[partition];
        for (int i = 0;i < partition; i++){
            this->head[i] = 0;
        }
        this->mainC = c;
    }

    void push_back(int partition, int el){
        this->container[partition * this->bufSize + this->head[partition]] = el;
        this->head[partition] += 1;
    }

    void insert(int partition, int * els, int n){
        int * p = this->getPartition(partition);
        int& h = this->head[partition];
        p = p + h;
        for(int i = 0; i < n; i++){
            if (h == this->bufSize){
                cout << "Partition: " << partition <<" Buffer Overflow" << endl;
                exit(1);
            }
            //p[h++] = els[i];
            _mm_stream_si32(p, els[i]);
            p++;
            h++;
        }
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

    ~custom_container(){
        free(this->container);
        free(this->head);
    }
};