hash_approach: hash_approach.cpp hash_approach.h
	g++ -c hash_approach.cpp -o hash_approach.o
build: main.cpp hash_approach
	g++ main.cpp hash_approach.o -o main -O3 -march=native 
build_debug: main.cpp hash_approach.cpp
	g++ main.cpp hash_approach.cpp -o main_debug -g
build_generator:
	g++ igenerator.cpp -o generate
run: 
	./main ${arg} file1 file2 =
generate:
	./generate ${arg}
stat:
	sudo perf stat -e LLC-loads,LLC-load-misses,branch-misses,cycles,instructions,cache-references,cache-misses,L1-dcache-load-misses,L1-dcache-loads,l2_rqsts.references,l2_rqsts.miss,dtlb_load_misses.miss_causes_a_walk,dTLB-loads,dTLB-load-misses,dTLB-prefetch-misses,L1-icache-load-misses ./main ${arg} ${func_name} file1 file2 =

record:
	sudo perf record -o ${arg1} ${func_name} -g -F 5000 -e dtlb_load_misses.miss_causes_a_walk,dTLB-loads,dTLB-load-misses,LLC-loads,LLC-load-misses,branch-misses,cycles,instructions,cache-references,cache-misses,bus-cycles,l2_rqsts.references,l2_rqsts.miss,L1-dcache-load-misses,cycle_activity.cycles_l2_miss ./main ${arg} file1 file2 =

clean:
	rm main.o

