#include <iostream>
#include "odbcwrap.hpp"

int main(int, char**) {
    try {
        auto conn = odbcwrap::odbc_connection::make();
        conn->setDsn("JMC_ALTI");
        conn->setUid("jmc");
        conn->setPwd("dbadmin");
        conn->connectDB();

        auto sql = conn->sql();

        sql->directExecute("SELECT 100;");
        int data = 0;
        sql->bindCol(1, &data);
        auto ret = sql->fetch();
        std::cout << "ret : " << std::boolalpha << ret << std::endl;
        std::cout << "data : " << data << std::endl;

        
    } catch (const odbcwrap::odbc_error & err) {
        std::cout << "odbc_error : " << err.what() << std::endl;
        
    } catch (const std::exception & err) {
        std::cout << "exception : " << err.what() << std::endl;
    } catch (...) {
        std::cout << "oops!" << std::endl;
    }

    return 0;
}

