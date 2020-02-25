# Cache-Conscious-Joins
Build process: make build (generates "main" as executable)

Run ./main num_of_element func_name file1 file2 operator partitionNumber(optional)

Example: ./main 10000 basicSort file1 file2 =

Available operator: "=" only equijoin is supported but this can be extended to >, < operator too.

Available func_name: basicNestedLoop, blockedNestedLoop, partitionedSort, basicSort, partitionedHash, basicHash

#Generate data
To generate file1 and file2, do: make build_generator and ./generate num_of_element

Note: Whatever number is provided as num_of_element is multiplied by 1000 so make sure you pass the right number. for eg: 10000 is 1M

Example: ./generate 10000 generates unique 1M elements from 0 to 10M 

# Get Statistics 
make stat arg=num_element func_name=function_name

Example: make stat arg=10000 func_name=basicSort
		
