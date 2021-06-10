# odbcwrap
this is very simple library
C++11 DB client library wrapped ODBC    
just hpp header file

# function list
1. db connection
2. auto commit / commit / rollback
3. query execute directly
4. query prepare and execute for reuse
5. query with binding parameter
6. db connection close
7. exception
8. connection pool (#TODO)

# example

## connection
``` cpp
#include "odbwrap.hpp"

auto conn = odbcwrap::odbc_connection::make();
conn->init();
conn->setDsn(dsn);
conn->setUid(uid);
conn->setPwd(pwd);
conn->setAutoCommit(true);
conn->connectDB();

auto isAutoCommit = conn->getAutoCommit();
conn->setAutoCommmit(false);
conn->commit();
conn->rollback();

```

## ping
``` cpp
#include "odbwrap.hpp"

auto sql = conn->sql();
sql->directExecute("SELECT 1;");
sql->fetch();
```

## select & data type
``` cpp
#include "odbwrap.hpp"

odbcwrap::Int32 data1;
odbcwrap::UInt64 data2;
odbcwrap::Float data3;
odbcwrap::Char<20 + 1> data4;
odbcwrap::NullInt32 data5;
odbcwrap::NullChar<10 + 1> data6;

auto sql = conn->sql();
sql->directExecute("SELECT DATA1, DATA2, DATA3, DATA4, DATA5, DATA6 FROM TBL_TEST WHERE  \
    DATA1=? AND DATA2=? AND DATA3=? AND DATA4=? AND DATA5=? AND DATA6=?", 
    1, 2, 333.333f, "constchar", 4, std::string("stringdata"));
sql->bindCol(1, &data1);
sql->bindCol(2, &data2);
sql->bindCol(3, &data3);
sql->bindCol(4, &data4);
sql->bindCol(5, &data5);
sql->bindCol(6, &data6);

while(sql->fetch()){
    std::cout << data1.value << std::endl;
    std::cout << data2.value << std::endl;
    std::cout << data3.value << std::endl;
    std::cout << data4.value << std::endl;
    if(data5.Valid()) std::cout << data5.value << std::endl;
    if(data6.Valid()) std::cout << data6.value << std::endl;
    
}

```

## insert/update/delete 
``` cpp
auto sql = conn->sql();
sql->directExecute(
    "INSERT INTO TBL_TEST (DATA1, DATA2, DATA3, DATA4, DATA5, DATA6) VALUES (?, ?, ?, ?, ?, ?)",
    1, 2, 333.333f, "constchar", 4, "stringdata");
cnt1 = sql->rowCount();
```

## disconnect / check connectcion
``` cpp
if(conn->getConnectionDead())
    conn->disconnectDB();
```
just use. noexcept
``` cpp
conn->disconnectDB();
```

## prepare query / reuse query / stmt handle
no binding query
``` cpp
auto sql = conn->sql();
sql.prepare("SELECT DATA1, DATA2, DATA3, DATA4 FROM TBL_TEST WHERE DATA1=1");

for(int i = 0 ; 1000; i++){
    sql->execute();
}
```
binding query
``` cpp
auto sql = conn->sql();
sql.prepare("SELECT DATA1, DATA2, DATA3, DATA4 FROM TBL_TEST WHERE DATA1=?");

for(int i = 0 ; 1000; i++){
    sql->bindExecute(i);
}
```

## dbcp
It is possible that use a dbcp for sychronized connection with [others queue library mpmc_bounded_queue](https://github.com/inkooboo/thread-pool-cpp/blob/af95dd88daa094f67bbd178b639c7282373a3b09/include/thread_pool/mpmc_bounded_queue.hpp)

see test source

# issue 
-   how to read VARCHAR(1) `type` ???    
VARCHAR 타입을 char로 받아올수는 없는걸까?
- how to bind parameter `null`??
파라미터 바인딩을 할때 null을 넣는 방법?

# todo list
1. connection pool
2. more test case
3. override method for odbcwrap::datatype `==`, `=`, `<<`, `>>` ??또 있나?


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
