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

        
        int data = 0;

        sql->prepare("SELECT 1000 from TSMS_SND_MSG WHERE MSG_SEQ=?");
        sql->bindExecute(0);
        sql->bindCol(1, &data);
        sql->fetch();
        std::cout << "data : " << data << std::endl;

        SQLFreeStmt(sql->stmt->stmt, SQL_CLOSE);
        int seq = 0;
        sql->bindExecute(seq);
        auto ret2 = sql->fetch();
        std::cout << "ret2:" << ret2 << ", data : " << data << std::endl;

        SQLFreeStmt(sql->stmt->stmt, SQL_CLOSE);
        sql->bindExecute("0");
        auto ret3 = sql->fetch();
        std::cout << "ret3:" << ret3 << ", data : " << data << std::endl;

        {
            const char * szSeq = "0";
            SQLFreeStmt(sql->stmt->stmt, SQL_CLOSE);
            sql->bindExecute(szSeq);
            auto ret = sql->fetch();
            std::cout << "ret:" << ret << ", data : " << data << std::endl;
        }


        {
            SQLFreeStmt(sql->stmt->stmt, SQL_CLOSE);
            sql->bindExecute(std::string("0"));
            auto ret = sql->fetch();
            std::cout << "ret:" << ret << ", data : " << data << std::endl;
        }

        {
            char buf[1024] = "0";
            SQLFreeStmt(sql->stmt->stmt, SQL_CLOSE);
            sql->bindExecute(buf);
            auto ret = sql->fetch();
            std::cout << "ret:" << ret << ", data : " << data << std::endl;
        }


        {
            std::string seq = "0";
            SQLFreeStmt(sql->stmt->stmt, SQL_CLOSE);
            sql->bindExecute(seq);
            auto ret = sql->fetch();
            std::cout << "ret:" << ret << ", data : " << data << std::endl;
        }
        {
            unsigned int seq = 0;
            SQLFreeStmt(sql->stmt->stmt, SQL_CLOSE);
            sql->bindExecute(seq);
            auto ret = sql->fetch();
            std::cout << "ret:" << ret << ", data : " << data << std::endl;
        }

        {
            double seq = 0;
            SQLFreeStmt(sql->stmt->stmt, SQL_CLOSE);
            sql->bindExecute(seq);
            auto ret = sql->fetch();
            std::cout << "ret:" << ret << ", data : " << data << std::endl;
        }

        {
            float seq = 0;
            SQLFreeStmt(sql->stmt->stmt, SQL_CLOSE);
            sql->bindExecute(seq);
            auto ret = sql->fetch();
            std::cout << "ret:" << ret << ", data : " << data << std::endl;
        }

        {
            short seq = 0;
            SQLFreeStmt(sql->stmt->stmt, SQL_CLOSE);
            sql->bindExecute(seq);
            auto ret = sql->fetch();
            std::cout << "ret:" << ret << ", data : " << data << std::endl;
        }
        {
            unsigned short seq = 0;
            SQLFreeStmt(sql->stmt->stmt, SQL_CLOSE);
            sql->bindExecute(seq);
            auto ret = sql->fetch();
            std::cout << "ret:" << ret << ", data : " << data << std::endl;
        }

        {

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

