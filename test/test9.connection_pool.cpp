#include <iostream>
#include <future>
#include "../odbcwrap.hpp"
#include "thread_pool.hpp"


#define QUERY_SELECT_WHERE "\
SELECT MYKEY \
FROM mytable \
WHERE MYKEY = ?"



auto dbcp = tp::MPMCBoundedQueue<std::shared_ptr<odbcwrap::odbc_connection>>(4); // mpmc_bounded_queue

// ./test.select JMC_ALTI jmc dbadmin 5
int main(int argc, char* argv[]) {
    if(argc < 5) { 
        std::cout << "you must input parameters" << std::endl;
        std::cout << "example : ./test.connection_pool 16 DSN_NAME db_id db_pwd 5" << std::endl;
        return 0;
    } else {
        std::cout << "size:" << argv[1] << ",dsn:" << argv[2] << ",uid:" << argv[3] << ",pwd:" << argv[4] << ",sleep_second:" << argv[5] << std::endl;
    }

    size_t size = std::stoi(argv[1]);
    std::string dsn = argv[2];
    std::string uid = argv[3];
    std::string pwd = argv[4];
    int sleep = std::stoi(argv[5]);
    
    try {
        for(int i = 0 ; i < 4 ; i++){
            auto conn = odbcwrap::odbc_connection::make();
            conn->init();
            conn->setDsn(dsn);
            conn->setUid(uid);
            conn->setPwd(pwd);
            conn->connectDB(); // just sync connection.. It should be a asynchronous connection??

            dbcp.push(conn);
        }

        for(int i = 0 ; i < 4 ; i++){
            std::shared_ptr<odbcwrap::odbc_connection> conn = nullptr;
            if(dbcp.pop(conn)){
                auto sql = conn->sql();
                odbcwrap::Int16 ping;
                ping.value = 0;
                sql->directExecute("SELECT 1");
                sql->bindCol(1, &ping);
                sql->fetch();
                std::cout << "ping:" << ping.value << std::endl;

                if(!dbcp.push(conn)){
                    throw std::runtime_error("fail push queue");
                }

            } else {
                throw std::runtime_error("fail pop queue");
            }
        }


        std::this_thread::sleep_for(std::chrono::seconds(sleep));
        
    } catch (const odbcwrap::odbc_error & err) {
        std::cout << "odbc_error : " << err.what() << std::endl;
    } catch (const std::exception & err) {
        std::cout << "exception : " << err.what() << std::endl;
    } catch (...) {
        std::cout << "oops!" << std::endl;
    }

    std::cout << "end..?" << std::endl;
    return 0;
}


