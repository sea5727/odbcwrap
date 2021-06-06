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

        const char * query = "INSERT INTO TSMS_SND_MSG (MSG_SEQ,SC_TIME,IN_TIME,SEND_TIME,SEND_ACK_TIME,REPORT_TIME,DELIVERY_TIME,EXPIRED_TIME,MSG_TYPE,SUB_MSG_TYPE,CALLER_NO,CALLBACK_NO,CALLEE_NO,ORG_CALLEE_NO,IN_SIP_URI,OUT_SIP_URI,MESSAGE,PROC_STS,PROC_RESULT,SERIAL_NUM,RPT_YN,USE_YN,RETRY_CNT,RV_DLV_TYPE,SD_DLV_TYPE,RD_VALUE,RN_RELATED_YN,IN_GROUP,IN_SECT,OUT_GROUP,OUT_SECT,ORDER_SEQ,QOR_N_ACQ,PREFIX_CODE,CONN_TYPE) VALUES (?,'SC_TIME','IN_TIME','SEND_TIME','SEND_ACK_TIME','REPORT_TIME','DELIVERY_TIME','EXPIRED_TIME',1,2,'CALLER_NO','CALLBACK_NO','CALLEE_NO','ORG_CALLEE_NO','IN_SIP_URI','OUT_SIP_URI','MESSAGE',3,4,'SERIAL_NUM',5,6,7,8,9,'RD_VALUE',10,11,12,13,14,15,16,17,'CONNTYPE');";
        // {
        //     sql->prepare(query);
        //     sql->bindExecute(1);
        //     auto row = sql->rowCount();
        //     std::cout << "row:" << row << std::endl;            
        // }


        {
            sql->prepare(query);
            sql->bindExecute(2);
            auto row = sql->rowCount();
            std::cout << "row:" << row << std::endl;            
        }
        {
            sql->prepare(query);
            sql->bindExecute(3);
            auto row = sql->rowCount();
            std::cout << "row:" << row << std::endl;            
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

