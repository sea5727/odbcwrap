#include <iostream>
#include "../odbcwrap.hpp"


#define QUERY_SELECT_WHERE "\
SELECT MYKEY \
FROM mytable \
WHERE MYKEY = ?"



// ./test.select JMC_ALTI jmc dbadmin 5
int main(int argc, char* argv[]) {
    if(argc < 5) { 
        std::cout << "you must input parameters" << std::endl;
        std::cout << "example : ./test.connection_pool 16 DSN_NAME db_id db_pwd 5" << std::endl;
        return 0;
    } else {
        std::cout << "dsn:" << argv[1] << ",uid:" << argv[2] << ",pwd:" << argv[3] << ",sleep_second:" << argv[4] << std::endl;
    }

    size_t size = std::stoi(argv[1]);
    std::string dsn = argv[2];
    std::string uid = argv[3];
    std::string pwd = argv[4];
    int sleep = std::stoi(argv[5]);

    try {
        auto pool = odbcwrap::odbc_connnection_pool::make(size, dsn, uid, pwd);
        auto conn = odbcwrap::odbc_connection::make();
        pool.pop(conn);
        // auto sql = conn->sql();
        // sql->directExecute("SELECT 1;");
        // sql->bindCol(1, &data);

        // sql->fetch();

        // std::cout << "data: " << data << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(sleep)); 
    } catch (const odbcwrap::odbc_error & err) {
        std::cout << "odbc_error : " << err.what() << std::endl;
    } catch (const std::exception & err) {
        std::cout << "exception : " << err.what() << std::endl;
    } catch (...) {
        std::cout << "oops!" << std::endl;
    }

    return 0;
}

