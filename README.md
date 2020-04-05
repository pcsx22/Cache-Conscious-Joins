# Cache-Conscious-Joins
** The cuda version of algorithm are present in the cuda folder.
# Cuda build
1. cd into cuda folder
2. Do make build. This generates "main_gpu" binary
3. Run ./main_gpu power_of_10 func_name file1 file2 operator numberOfThread(redundant but required) numberOfPartition

** make sure file1 and file2 exists 

Available func_name: partitionedSortParallel, sortParallel (un-partitioned), partitionedHashParallel


# CPU build
Build process: make build (generates "main" as executable)

Run ./main power_of_10 func_name file1 file2 operator numberOfThread numberOfPartition

Example: ./main 7 partitionedSortParallel file1 file2 = numberOfThread numberOfPartition
- The following command reads 10^7 elements from file1 and file2 and joins via parallel partitioned sort algorithm

- Available operator: "=" only equijoin is supported but this can be extended to >, < operator too.

Available func_name: basicNestedLoop, blockedNestedLoop, partitionedSortSerial, partitionedSortParallel, basicSort, partitionedHashSerial, partitionedHash, basicHash

# Generate data
To generate file1 and file2, do: make build_generator and ./generate power_of_10
- For example: ./generate 8. This command generates 2 set of 10^8 unique and random elements and writes them in files file1 and file2

# Get Statistics 
make stat arg=num_element func_name=function_name p=parition_number threads=thread_number

Example: make stat arg=7 func_name=partitionedSortParallel p=1021 threads=4
		
