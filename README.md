# odbcwrap

C++11 odbc wrapping library with just hpp file

# function list
1. db connection
2. auto commit / commit / rollback
3. query execute directly
4. query prepare and execute for reuse
5. query with binding parameter
6. db connection close
7. connection pool (#TODO)

# example

## connection
``` cpp
#include "odbwrap.hpp"


```

# issue 
-   how to read VARCHAR(1) type ???    
VARCHAR 타입을 char로 받아올수는 없는걸까?

# todo list
1. connection pool
2. more test case

# connection pool

[mpmc_bounded_queue](https://github.com/inkooboo/thread-pool-cpp/blob/af95dd88daa094f67bbd178b639c7282373a3b09/include/thread_pool/mpmc_bounded_queue.hpp) is from `inkooboo/thread-pool-cpp` git hub project

