#include <iostream>
#include "odbcwrap.hpp"

int main(int, char**) {
    try {
        auto conn = odbcwrap::odbc_connection::make();
        // conn->setDsn("JMC_ALTI");
        // conn->setUid("jmc");
        // conn->setPwd("dbadmin");
        // conn->connectDB();
        // std::this_thread::sleep_for(std::chrono::seconds(1));
        // conn->disconnectDB();
    } catch (const odbcwrap::odbc_error & err) {
        std::cout << "odbc_error : " << err.what() << std::endl;
        
    } catch (const std::exception & err) {
        std::cout << "exception : " << err.what() << std::endl;
    } catch (...) {
        std::cout << "oops!" << std::endl;
    }

    return 0;
}

