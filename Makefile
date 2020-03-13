build: hash_approach.h sort_approach.h
	g++ main.cpp general.h hash_approach.cpp sort_approach.cpp -o main -O3 -march=native -fopenmp
build_debug: hash_approach.h sort_approach.h
	g++ main.cpp general.h hash_approach.cpp sort_approach.cpp -o main_debug -g -fopenmp
build_generator:
	g++ igenerator.cpp -o generate
run: 
	./main ${arg} file1 file2 =
generate:
	./generate ${arg}
stat:
	sudo perf stat -e LLC-loads,LLC-load-misses,branch-misses,cycles,instructions,cache-references,cache-misses,L1-dcache-load-misses,L1-dcache-loads,l2_rqsts.references,l2_rqsts.miss,dtlb_load_misses.miss_causes_a_walk,dTLB-loads,dTLB-load-misses,dTLB-prefetch-misses,L1-icache-load-misses ./main ${arg} ${func_name} file1 file2 = ${p}

record:
	sudo perf record -o ${arg1} -g -F 5000 -e dtlb_load_misses.miss_causes_a_walk,dTLB-loads,dTLB-load-misses,LLC-loads,LLC-load-misses,branch-misses,cycles,instructions,cache-references,cache-misses,bus-cycles,l2_rqsts.references,l2_rqsts.miss,L1-dcache-load-misses,cycle_activity.cycles_l2_miss ./main ${arg} ${func_name} file1 file2 = ${p}

clean:
	rm main main_debug hash_approach.o sort_approach.o

