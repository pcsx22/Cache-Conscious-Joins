SIMD_HEADER_FILE=simd/simd_sort.h simd/sort_util.h simd/merge_util.h simd/utils.h simd/common.h
SIMD_DEP=simd/simd_sort.cpp simd/sort_util.cpp simd/merge_util.cpp simd/utils.cpp simd/common.cpp $(SIMD_HEADER_FILE)
B_FLAGS=-flto -march=native -mavx2 -O3 -fopenmp -DAVX2
SIMD_OBJECT_FILES=sort_util.o simd_sort.o merge_util.o utils.o common.o

all: simd_sort build
simd_sort:$(SIMD_DEP)
	g++ -c $(B_FLAGS) $(SIMD_DEP)
build: $(SIMD_OBJECT_FILES) hash_approach.h sort_approach.h general.h
	g++ $(SIMD_OBJECT_FILES) main.cpp general.h hash_approach.cpp sort_approach.cpp -o main -O3 -mavx2 -march=native -Wall -fopenmp
build_debug: $(SIMD_OBJECT_FILES) hash_approach.h sort_approach.h general.h
	g++ $(SIMD_OBJECT_FILES) main.cpp general.h hash_approach.cpp sort_approach.cpp -o main_debug -march=native -mavx2 -Wall -g -fopenmp
build_generator:
	g++ igenerator.cpp -o generate
run: 
	./main ${arg} file1 file2 =
generate:
	./generate ${arg}
stat:
	sudo perf stat -e LLC-loads,LLC-load-misses,branch-misses,cycles,instructions,cache-references,cache-misses,L1-dcache-load-misses,L1-dcache-loads,l2_rqsts.references,l2_rqsts.miss,dtlb_load_misses.miss_causes_a_walk,dTLB-loads,dTLB-load-misses,dTLB-prefetch-misses,L1-icache-load-misses ./main ${arg} ${func_name} file1 file2 = ${threads} ${p}

record:
	sudo perf record -o ${arg1} -g -F 5000 -e dtlb_load_misses.miss_causes_a_walk,dTLB-loads,dTLB-load-misses,LLC-loads,LLC-load-misses,branch-misses,cycles,instructions,cache-references,cache-misses,bus-cycles,l2_rqsts.references,l2_rqsts.miss,L1-dcache-load-misses,cycle_activity.cycles_l2_miss ./main ${arg} ${func_name} file1 file2 = 8 ${p}

clean:
	rm main main_debug hash_approach.o sort_approach.o

