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

        odbcwrap::Bool data1;
        odbcwrap::Int16 data2;
        odbcwrap::UInt16 data3;
        odbcwrap::Int32 data4;
        odbcwrap::UInt32 data5;
        odbcwrap::Int64 data6;
        odbcwrap::UInt64 data7;
        odbcwrap::Float data8;
        odbcwrap::Double data9;
        odbcwrap::Char<16 + 1> data10;
        odbcwrap::Char<16 + 1> data11;

        auto sql = conn->sql();
        
        sql->directExecute("SELECT DATA1, DATA2, DATA3, DATA4, DATA5, DATA6, DATA7, DATA8, DATA9, DATA10, DATA11 FROM TBL_TEST WHERE \
            DATA1=? AND DATA2=? AND DATA3=? AND DATA4=? AND DATA5=? AND DATA6=? AND DATA7=? AND DATA8=? AND DATA9=? AND DATA10=? AND DATA11=?;", 
            true, 1, 2, 3, 4, 5, 6, 7, 8, 9, "test_const_char", std::string("test_string"));
        sql->bindCol(1, &data1);
        sql->bindCol(2, &data2);
        sql->bindCol(3, &data3);
        sql->bindCol(4, &data4);
        sql->bindCol(5, &data5);
        sql->bindCol(6, &data6);
        sql->bindCol(7, &data7);
        sql->bindCol(8, &data8);
        sql->bindCol(9, &data9);
        sql->bindCol(10, &data10);
        sql->bindCol(11, &data11);

        while(sql->fetch()){
            
            std::cout << "data1: " << data1.value << std::endl;
            std::cout << "data2: " << data2.value << std::endl;
            std::cout << "data3: " << data3.value << std::endl;
            std::cout << "data4: " << data4.value << std::endl;
            std::cout << "data5: " << data5.value << std::endl;
            std::cout << "data6: " << data6.value << std::endl;
            std::cout << "data7: " << data7.value << std::endl;
            std::cout << "data8: " << data8.value << std::endl;
            std::cout << "data9: " << data9.value << std::endl;
            std::cout << "data10: " << data10.value << std::endl;
            std::cout << "data11: " << data11.value << std::endl;
        }

        
    } catch (const odbcwrap::odbc_error & err) {
        std::cout << "odbc_error : " << err.what() << std::endl;
    } catch (const std::exception & err) {
        std::cout << "exception : " << err.what() << std::endl;
    } catch (...) {
        std::cout << "oops!" << std::endl;
    }

    return 0;
}

