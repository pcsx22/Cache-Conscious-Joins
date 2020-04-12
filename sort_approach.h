void basicSort(int * col1, int * col2, int c1, int c2);
void partitionedSortSerial(int * c1, int * c2, int n1, int n2, int partitions);
void partitionedSortParallel(int * c1, int * c2, int n1, int n2, int partitions); 
void partitionedSortParallelSIMD(int * c1, int * c2, int n1, int n2, int partitions); 
void partitionedSortSerialSIMD(int * c1, int * c2, int n1, int n2, int partitions);