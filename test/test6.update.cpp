#include <iostream>
#include "../odbcwrap.hpp"


#define QUERY_SELECT "\
SELECT MYKEY, NUM_SMALL, NUM_BIG, NUM_NULL, VARCHAR_NULL, VARCHAR_NOT_NULL, CHAR_NULL, CHAR_NOT_NULL, FLOAT_NULL, FLOAT_NOT_NULL, DOUBLE_NULL, DOUBLE_NOT_NULL \
FROM mytable;"

#define QUERY_UPDATE_WHERE_3 "\
UPDATE mytable SET VARCHAR_NULL='NEW_UPDATE' WHERE MYKEY=? AND NUM_SMALL=? AND VARCHAR_NOT_NULL=? AND FLOAT_NOT_NULL=? AND DOUBLE_NOT_NULL=? \
"

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


        odbcwrap::Int32 MYKEY;
        odbcwrap::Int16 NUM_SMALL;
        odbcwrap::Int64 NUM_BIG;
        odbcwrap::NullInt32 NUM_NULL;
        odbcwrap::NullChar<32 + 1> VARCHAR_NULL;
        odbcwrap::Char<32 + 1> VARCHAR_NOT_NULL;
        odbcwrap::NullChar<1 + 1> CHAR_NULL;
        odbcwrap::Char<1 + 1> CHAR_NOT_NULL;
        odbcwrap::NullFloat FLOAT_NULL;
        odbcwrap::Float FLOAT_NOT_NULL;
        odbcwrap::NullDouble DOUBLE_NULL;
        odbcwrap::Double DOUBLE_NOT_NULL;

        int data = 0;
        int cnt = 0;
        auto sql = conn->sql();
        sql->directExecute(QUERY_UPDATE_WHERE_3, 2, 3333, "HELLO", 123.123f, 456.456);
        cnt = sql->updateRowCount();
        std::cout << "count:" << cnt << std::endl;

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

