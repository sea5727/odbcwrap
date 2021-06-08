#include <iostream>
#include "../odbcwrap.hpp"



#define QUERY_INSERT_1_FOR_NULL "\
INSERT INTO mytable \
(MYKEY, NUM_SMALL, NUM_BIG, NUM_NULL, VARCHAR_NULL, VARCHAR_NOT_NULL, CHAR_NULL, CHAR_NOT_NULL, FLOAT_NULL, FLOAT_NOT_NULL, DOUBLE_NULL, DOUBLE_NOT_NULL) \
VALUES \
(0, 1111, 9999999999, NULL, NULL, 'HELLO',  NULL, 'a', NULL, 123.123, NULL, 456.456) \
"
#define QUERY_INSERT_2_NOT_NULL "\
INSERT INTO mytable \
(MYKEY, NUM_SMALL, NUM_BIG, NUM_NULL, VARCHAR_NULL, VARCHAR_NOT_NULL, CHAR_NULL, CHAR_NOT_NULL, FLOAT_NULL, FLOAT_NOT_NULL, DOUBLE_NULL, DOUBLE_NOT_NULL) \
VALUES \
(1, 2222, 8888888888, 1000, 'NULCHAR', 'HELLO',  'c' , 'a', 999.999, 123.123, 888.888, 456.456) \
"

#define QUERY_INSERT_3_NOT_NULL "\
INSERT INTO mytable \
(MYKEY, NUM_SMALL, NUM_BIG, NUM_NULL, VARCHAR_NULL, VARCHAR_NOT_NULL, CHAR_NULL, CHAR_NOT_NULL, FLOAT_NULL, FLOAT_NOT_NULL, DOUBLE_NULL, DOUBLE_NOT_NULL) \
VALUES \
(2, 3333, 8888888888, 1000, 'NULCHAR', 'HELLO',  'c' , 'a', 999.999, 123.123, 888.888, 456.456) \
"


// ./test.ping JMC_ALTI jmc dbadmin 5
int main(int argc, char* argv[]) {
    if(argc < 5) { 
        std::cout << "you must input parameters" << std::endl;
        std::cout << "example : ./test.insert DSN_NAME db_id db_pwd 5" << std::endl;
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


        int data = 0;
        int cnt1 = 0;
        int cnt2 = 0;
        int cnt3 = 0;
        auto sql = conn->sql();
        sql->directExecute(QUERY_INSERT_1_FOR_NULL);
        cnt1 = sql->insertRowCount();
        std::cout << "insert:" << cnt1 << std::endl;
        sql->directExecute(QUERY_INSERT_2_NOT_NULL);
        cnt2 = sql->insertRowCount();
        std::cout << "insert:" << cnt2 << std::endl;
        sql->directExecute(QUERY_INSERT_3_NOT_NULL);
        cnt3 = sql->insertRowCount();
        std::cout << "insert:" << cnt3 << std::endl;

        std::this_thread::sleep_for(std::chrono::seconds(sleep)); 
        conn->disconnectDB();
    } catch (const odbcwrap::odbc_error & err) {
        std::cout << "odbc_error : " << err.what() << std::endl;
    } catch (const std::exception & err) {
        std::cout << "exception : " << err.what() << std::endl;
    } catch (...) {
        std::cout << "oops!" << std::endl;
    }

    return 0;
}

