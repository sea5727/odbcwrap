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
        std::cout << "example : ./test.ping DSN_NAME db_id db_pwd 5" << std::endl;
        return 0;
    } else {
        std::cout << "dsn:" << argv[1] << ",uid:" << argv[2] << ",pwd:" << argv[3] << ",sleep_second:" << argv[4] << std::endl;
    }

    std::string dsn = argv[1];
    std::string uid = argv[2];
    std::string pwd = argv[3];
    int sleep = std::stoi(argv[4]);

    try {
        auto conn = odbcwrap::odbc_connection::make();
        conn->setDsn(dsn);
        conn->setUid(uid);
        conn->setPwd(pwd);
        conn->connectDB();
        if(conn->getConnectionDead())
            conn->disconnectDB();
        // you can command netstat 
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

