#include <iostream>
#include "odbcwrap.hpp"


int main(int, char**) {
    std::shared_ptr<odbcwrap::odbc_connection> conn = nullptr;
    while(1) {
        try {
            conn = odbcwrap::odbc_connection::make();
            conn->setDsn("JMC_ALTI");
            conn->setUid("jmc");
            conn->setPwd("dbadmin");
            conn->connectDB();

            auto sql = conn->sql();
            odbcwrap::Int16 data1;
            sql->prepare("SELECT 1 FROM TSMS_SND_MSG;");
            sql->execute();
            sql->bindCol(1, data1);
            int idx = 0;
            while(sql->fetch()){
                std::cout << idx++ << "query succ\n";
            }


            std::this_thread::sleep_for(std::chrono::milliseconds(10));

        } catch (const odbcwrap::odbc_error & err) {
            std::cout << "odbc_error : " << err.what() << std::endl;
            
        } catch (const std::exception & err) {
            std::cout << "exception : " << err.what() << std::endl;
        } catch (...) {
            std::cout << "oops!" << std::endl;
        }

    }



    return 0;
}

