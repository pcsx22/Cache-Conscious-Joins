HEADER_FILE=simd_sort.h sort_util.h merge_util.h utils.h common.h
DEP=simd_sort.cpp sort_util.cpp merge_util.cpp utils.cpp common.cpp $(HEADER_FILE)
B_FLAGS=-flto -march=native -mavx2 -fopenmp -DAVX2

build:$(DEP) myTest.cpp
	g++ $(B_FLAGS) $(DEP) myTest.cpp -o myTest -g