#include<unordered_set>
#include<string>
#include<random>
#include<fstream>
#include<vector>
#include<iostream>
#include<algorithm>
#include<math.h>
using namespace std;

void writeToFile(vector<long> & v, char * filename){
    ofstream s;
    s.open(filename, std::ofstream::out | std::ofstream::trunc);
    for (long i = 0;i<v.size();i++){
        s << v[i];
        s << " ";
    }
    s.close();
    return;
}

int main(int argc, char ** argv){
    long inputSize = 1000;
    if (argc == 2){
        inputSize = stoi(argv[1]);
    }
    
    random_device rd;
    mt19937 mt(rd());
    inputSize = pow(10, inputSize);
    vector<long> op(inputSize);
    uniform_real_distribution<double> dist(0, 5*inputSize);
    unordered_set<long> seen;
    std::unordered_set<long>::iterator it;
    long x;
    for(long i= 0; i < inputSize; i++){
        x = dist(mt);
        while (seen.find(x) != seen.end()){
            x = dist(mt);
        }
        op[i] = x;
        seen.insert(x);
    }
    random_device rd1;
    mt19937 mt1(rd1());
    uniform_real_distribution<double> dist1(1, inputSize * 10);
    vector<long> op1(inputSize);
    unordered_set<long> seen1;
    for(long i = 0;i < inputSize; i++){
        x = dist1(mt1);
        while (seen1.find(x) != seen1.end()){
            x = dist1(mt1);
        }
        op1[i] = x;
        seen1.insert(x);
    }
    writeToFile(op, "file1");
    writeToFile(op1, "file2");
}