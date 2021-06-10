# odbcwrap

C++11 DB client library wrapped ODBC    
just hpp header file

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

# etc
SQLAllocHandle : ODBC Handle을 할당
SQLExecDirect : 쿼리문 하나를 전송, 가장 빠른 동작
SQLPrepare : Stmt 핸들러에 쿼리를 저장.
SQLBindParameter : 바인드 파라미터를 저장 '?' 에 들어갈 변수를 저장한다.
SQLFetch : `SELECT` 쿼리결과 결과를 bind된 파라미터 주소에 저장한다. (1row)
SQLBindCol : fetch시 조회결과를 담을 주소를 설정한다.
SQLGetData : SQLBindCol 과 반대로 fetch 후 1회성으로 데이터를 받는다.
SQLNumREsultCols : `SELECT` 쿼리 결과의 `Column` 개수
SQLRowCount : `INSERT/UPDATE/DELETE` 쿼리 결과의 row수

handler : 
HENV : Odbc 버전 정보 핸들러
HDBC : DB 연결핸들러, HENV와 HDBC는 1:N 의 관계
HSTMT : 쿼리문 핸들러, HDBC와 HSTMT는 1:N 관계
