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
        sql->prepare("SELECT 1 FROM TSMS_SND_MSG;");
        sql->execute();
        int idx = 0;
        while(sql->fetch()){
            std::cout << idx++ << "query succ\n";
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

