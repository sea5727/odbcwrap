#pragma once

#include <memory>
#include <string>
#include <iostream>
#include <tuple>
#include <sql.h>
#include <sqlext.h>
#include <functional>
#include <typeinfo> 
#include <string.h>
#include <vector>
#include <exception>
#include <sstream>
#include <utility>
#include <thread>
#include <chrono>
#include <mutex>
#include <atomic>
#include <type_traits>

namespace odbcwrap {
#define MAKE_ODBCWRAP_TYPE_NOT_NULL(name, type) \
class name { \
public: \
    type value; \
}
MAKE_ODBCWRAP_TYPE_NOT_NULL(Bool, bool);
MAKE_ODBCWRAP_TYPE_NOT_NULL(Int16, int16_t);
MAKE_ODBCWRAP_TYPE_NOT_NULL(UInt16, uint16_t);
MAKE_ODBCWRAP_TYPE_NOT_NULL(Int32, int32_t);
MAKE_ODBCWRAP_TYPE_NOT_NULL(UInt32, uint32_t);
MAKE_ODBCWRAP_TYPE_NOT_NULL(Int64, int64_t);
MAKE_ODBCWRAP_TYPE_NOT_NULL(UInt64, uint64_t);
MAKE_ODBCWRAP_TYPE_NOT_NULL(Float, float);
MAKE_ODBCWRAP_TYPE_NOT_NULL(Double, double);
#undef MAKE_ODBCWRAP_TYPE_NOT_NULL

#define MAKE_ODBCWRAP_TYPE_NULLABLE(name, type) \
class name { \
public: \
    type value; \
    bool valid() { return len!=0; }; \
    long len; \
}
MAKE_ODBCWRAP_TYPE_NULLABLE(NullBool, bool);
MAKE_ODBCWRAP_TYPE_NULLABLE(NullInt16, int16_t);
MAKE_ODBCWRAP_TYPE_NULLABLE(NullUInt16, uint16_t);
MAKE_ODBCWRAP_TYPE_NULLABLE(NullInt32, int32_t);
MAKE_ODBCWRAP_TYPE_NULLABLE(NullUInt32, uint32_t);
MAKE_ODBCWRAP_TYPE_NULLABLE(NullInt64, int64_t);
MAKE_ODBCWRAP_TYPE_NULLABLE(NullUInt64, uint64_t);
MAKE_ODBCWRAP_TYPE_NULLABLE(NullFloat, float);
MAKE_ODBCWRAP_TYPE_NULLABLE(NullDouble, double);
#undef MAKE_ODBCWRAP_TYPE_NULLABLE
    template <int N>
    class Char {
    public:
        char value[N] = {0,};
        long len;
    };
    template <int N>
    class NullChar {
    public:
        char value[N] = {0,};
        bool valid() { return len > 0; }
        long len;
    };
}

namespace odbcwrap {
    class odbc_error : public std::exception {
        std::string desc = "";
    public:
        std::string state = "";
        std::string text = "";
        odbc_error(const std::string & desc) 
            : desc(desc) {}
        odbc_error(SQLSMALLINT type, SQLHANDLE handle, SQLRETURN status) {
            if (status == SQL_INVALID_HANDLE) {
                switch(type) {
                    case SQL_HANDLE_ENV: desc = "SQL_INVALID_HANDLE : env";
                        break;
                    case SQL_HANDLE_DBC: desc = "SQL_INVALID_HANDLE : dbc";
                        break;
                    case SQL_HANDLE_STMT: desc = "SQL_INVALID_HANDLE : stmt";
                        break;
                    default: desc = "SQL_INVALID_HANDLE : unknowns";
                        break;
                }
            } else {

                SQLINTEGER      native;
                char            bufstate[32 ] = {0,};
                char            buftext [512] = {0,};
                SQLSMALLINT     len;
                SQLRETURN       ret;
                SQLLEN numRecs  = 0;
                SQLGetDiagField(type, handle, 0, SQL_DIAG_NUMBER, &numRecs, 0, 0);
                for(int i = 1 ; i <= numRecs; ++i){
                    ret = SQLGetDiagRec(type, 
                                        handle, 
                                        i, 
                                        (SQLCHAR *)bufstate, 
                                        &native, 
                                        (SQLCHAR *)buftext,
                                        sizeof(buftext), 
                                        &len );
                    if(ret != SQL_SUCCESS) {
                        desc = "unknowns error";
                        break;
                    } else {
                        state = bufstate;
                        text = buftext;
                        desc = "[" + state + "]" + text;
                        break; // todo next?
                    }                    
                }
            }
        }
        const char* what() const noexcept override {
            return desc.c_str();
        }
    };
}

namespace odbcwrap {
    class odbc_env {
    private:
        odbc_env() /* throwable */ {
            if(env == SQL_NULL_HENV){
                SQLRETURN status = 0;
                status = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &env);
                if(status != SQL_SUCCESS){
                    throw odbc_error("failure SQLAllocHandle SQL_HANDLE_ENV");
                }
                status = SQLSetEnvAttr(env, SQL_ATTR_ODBC_VERSION, (void *) SQL_OV_ODBC3, 0);
                if(status != SQL_SUCCESS){
                    SQLFreeHandle(SQL_HANDLE_ENV, env);
                    env = SQL_NULL_HENV;
                    throw odbc_error("failure SQLSetEnvAttr SQL_OV_ODBC3");
                }
            }
        }
        
    public:
        SQLHENV env = SQL_NULL_HENV;
        static odbc_env * getInstance() throw() {
            static odbc_env instance;
            return & instance;
        }
    };
}

namespace odbcwrap {
    class odbc_hdbc {
    public:
        SQLHDBC dbc = SQL_NULL_HDBC;
        odbc_hdbc() = default;
        odbc_hdbc(SQLHDBC dbc) : dbc(dbc) {} 
        odbc_hdbc(odbc_hdbc &&rhs) {
            dbc = rhs.dbc;
            rhs.dbc = SQL_NULL_HDBC;
        } 
        ~odbc_hdbc() {
            if(dbc != SQL_NULL_HDBC) {
                SQLDisconnect(dbc);
                SQLFreeConnect(dbc);
                dbc = SQL_NULL_HDBC;
            }
        }
    };
}

namespace odbcwrap {
    class odbc_hstmt {
    public:
        SQLHSTMT stmt = SQL_NULL_HSTMT;
        odbc_hstmt() = default;
        odbc_hstmt(SQLHSTMT stmt) : stmt(stmt) {} 
        odbc_hstmt(odbc_hstmt &&rhs) {
            stmt = rhs.stmt;
            rhs.stmt = SQL_NULL_HSTMT;
        } 
        ~odbc_hstmt(){
            if(stmt != SQL_NULL_HSTMT){
                SQLRETURN status = SQLFreeStmt(stmt, SQL_DROP);
                stmt = SQL_NULL_HSTMT;
            }                
        }
    };
}



namespace odbcwrap {
    class odbc_statement {
        // SQLExecute
    public:
        friend class odbc_connection;
        std::shared_ptr<odbc_hstmt> stmt = nullptr;
        odbc_statement(odbc_hdbc & dbc) /* throwable */ {
            
            SQLRETURN status = 0;
            SQLHDBC tmpstmt = SQL_NULL_HSTMT;
            status = SQLAllocHandle(SQL_HANDLE_STMT, dbc.dbc, &tmpstmt);
            if (status != SQL_SUCCESS){
                throw odbc_error(SQL_HANDLE_DBC, dbc.dbc, status);
            }
            stmt = std::make_shared<odbc_hstmt>(tmpstmt);
        }
    public:
        template <typename Param1, typename... Params>
        void
        directExecute(const std::string & query, const Param1 & p1, const Params&... rest) /* throwable */ { 
            bindParams(1, p1, rest...);
            directExecute(query);
        }

        void
        directExecute(const std::string & query) /* throwable */ {
            // SQLFreeStmt(stmt->stmt, SQL_CLOSE);
            SQLRETURN status = SQLExecDirect(stmt->stmt, (SQLCHAR *)query.c_str(), SQL_NTS);
            if( status != SQL_SUCCESS && status != SQL_NO_DATA/*for insert/update/delete query */){
                throw odbc_error(SQL_HANDLE_STMT, stmt->stmt, status);
            }
        }

        void
        prepare(const std::string & query) {
            SQLRETURN status = SQLPrepare( stmt->stmt, (SQLCHAR*)query.c_str(), SQL_NTS);
            if(status != SQL_SUCCESS){
                throw odbc_error(SQL_HANDLE_STMT, stmt->stmt, status);
            }
        }

        template <typename Param1, typename... Params>
        void
        bindExecute(const Param1 & p1, const Params&... rest) {
            bindParams(1, p1, rest...);
            SQLRETURN status = SQLExecute(stmt->stmt);  
            if( status != SQL_SUCCESS){
                throw odbc_error(SQL_HANDLE_STMT, stmt->stmt, status);
            }
        }


        void
        execute() {
            
            SQLRETURN status = SQLExecute(stmt->stmt);  
            if( status != SQL_SUCCESS){
                throw odbc_error(SQL_HANDLE_STMT, stmt->stmt, status);
            }
        }

        void
        bind(const int idx, const int16_t & param) /* throwable */ {
            SQLRETURN status = SQL_SUCCESS;

            status = SQLBindParameter( stmt->stmt,
                                    idx,
                                    SQL_PARAM_INPUT,
                                    SQL_C_SSHORT,
                                    SQL_DECIMAL,
                                    0,
                                    0,
                                    (SQLPOINTER)&param,
                                    sizeof(param),
                                    NULL );
            if(status != SQL_SUCCESS){
                throw odbc_error(SQL_HANDLE_STMT, stmt->stmt, status);
            }
        }

        void
        bind(const int idx, const uint16_t & param) /* throwable */ {
            SQLRETURN status = SQL_SUCCESS;

            status = SQLBindParameter( stmt->stmt,
                                    idx,
                                    SQL_PARAM_INPUT,
                                    SQL_C_USHORT,
                                    SQL_DECIMAL,
                                    0,
                                    0,
                                    (SQLPOINTER)&param,
                                    sizeof(param),
                                    NULL );
            if(status != SQL_SUCCESS){
                throw odbc_error(SQL_HANDLE_STMT, stmt->stmt, status);
            }
        }
        void
        bind(const int idx, const int & param) /* throwable */ {
            SQLRETURN status = SQL_SUCCESS;

            status = SQLBindParameter( stmt->stmt,
                                    idx,
                                    SQL_PARAM_INPUT,
                                    SQL_C_SLONG,
                                    SQL_DECIMAL,
                                    0,
                                    0,
                                    (SQLPOINTER)&param,
                                    sizeof(param),
                                    NULL );
            if(status != SQL_SUCCESS){
                throw odbc_error(SQL_HANDLE_STMT, stmt->stmt, status);
            }
        }

        void
        bind(const int idx, const unsigned int & param) /* throwable */ {
            SQLRETURN status = SQL_SUCCESS;

            status = SQLBindParameter( stmt->stmt,
                                    idx,
                                    SQL_PARAM_INPUT,
                                    SQL_C_ULONG,
                                    SQL_DECIMAL,
                                    0,
                                    0,
                                    (SQLPOINTER)&param,
                                    sizeof(param),
                                    NULL );
            if(status != SQL_SUCCESS){
                throw odbc_error(SQL_HANDLE_STMT, stmt->stmt, status);
            }
        }

        void
        bind(const int idx, const float & param) /* throwable */ {
            SQLRETURN status = SQL_SUCCESS;

            status = SQLBindParameter( stmt->stmt,
                                    idx,
                                    SQL_PARAM_INPUT,
                                    SQL_C_FLOAT,
                                    SQL_DECIMAL,
                                    0,
                                    0,
                                    (SQLPOINTER)&param,
                                    sizeof(param),
                                    NULL );
            if(status != SQL_SUCCESS){
                throw odbc_error(SQL_HANDLE_STMT, stmt->stmt, status);
            }
        }

        void
        bind(const int idx, const double & param) /* throwable */ {
            SQLRETURN status = SQL_SUCCESS;

            status = SQLBindParameter( stmt->stmt,
                                    idx,
                                    SQL_PARAM_INPUT,
                                    SQL_C_DOUBLE,
                                    SQL_DECIMAL,
                                    0,
                                    0,
                                    (SQLPOINTER)&param,
                                    sizeof(param),
                                    NULL );
            if(status != SQL_SUCCESS){
                throw odbc_error(SQL_HANDLE_STMT, stmt->stmt, status);
            }
        }


        template< unsigned N >
        void bind(const int idx, const char (&param)[N] ) /* throwable */ {
            SQLRETURN status = SQL_SUCCESS;
            status = SQLBindParameter( stmt->stmt,
                                    idx,
                                    SQL_PARAM_INPUT,
                                    SQL_C_CHAR,
                                    SQL_CHAR,
                                    0,
                                    0,
                                    (SQLPOINTER)param,
                                    strlen(param),
                                    NULL );
            if(status != SQL_SUCCESS){
                throw odbc_error(SQL_HANDLE_STMT, stmt->stmt, status);
            }
        }


        void bind(const int idx, const std::string & param) {
            SQLRETURN status = 1;
            status = SQLBindParameter( stmt->stmt,
                                    idx,
                                    SQL_PARAM_INPUT,
                                    SQL_C_CHAR,
                                    SQL_CHAR,
                                    0,
                                    0,
                                    (SQLPOINTER)param.c_str(),
                                    param.length(),
                                    NULL );
            if(status != SQL_SUCCESS){
                throw odbc_error(SQL_HANDLE_STMT, stmt->stmt, status);
            }
        }

        template <typename Param1>
        void
        bindParams(int idx, const Param1 & param1) {
            bind(idx, param1);
        }
        template <typename Param1, typename... Params>
        void
        bindParams(int idx, const Param1 & param1, const Params&... rest) {
            bind(idx, param1);
            bindParams(++idx, rest...);
        }

#define MAKE_BIND_COL_DEF_TYPE_NOT_NULL(cpptype, sql_type) \
void \
bindCol (const int index, cpptype * buffer) { \
    SQLRETURN status = SQLBindCol(stmt->stmt, index, sql_type, buffer, 0, NULL); \
    if (status != SQL_SUCCESS) { \
        throw odbc_error(SQL_HANDLE_STMT, stmt->stmt, status); \
    } \
}

MAKE_BIND_COL_DEF_TYPE_NOT_NULL(bool, SQL_C_USHORT)
MAKE_BIND_COL_DEF_TYPE_NOT_NULL(int16_t, SQL_C_SSHORT)
MAKE_BIND_COL_DEF_TYPE_NOT_NULL(uint16_t, SQL_C_USHORT)
MAKE_BIND_COL_DEF_TYPE_NOT_NULL(int, SQL_C_SLONG)
MAKE_BIND_COL_DEF_TYPE_NOT_NULL(unsigned int , SQL_C_ULONG)
MAKE_BIND_COL_DEF_TYPE_NOT_NULL(int64_t, SQL_C_SBIGINT)
MAKE_BIND_COL_DEF_TYPE_NOT_NULL(uint64_t, SQL_C_UBIGINT)
MAKE_BIND_COL_DEF_TYPE_NOT_NULL(float, SQL_C_FLOAT)
MAKE_BIND_COL_DEF_TYPE_NOT_NULL(double, SQL_C_DOUBLE)
#undef MAKE_BIND_COL_DEF_TYPE_NOT_NULL

#define MAKE_BIND_COL_ODBCWRAP_TYPE_NOT_NULL(odbcwraptype, sql_type) \
void \
bindCol (const int index, odbcwraptype * buffer) { \
    SQLRETURN status = SQLBindCol(stmt->stmt, index, sql_type, &buffer->value, 0, NULL); \
    if (status != SQL_SUCCESS) { \
        throw odbc_error(SQL_HANDLE_STMT, stmt->stmt, status); \
    } \
}
MAKE_BIND_COL_ODBCWRAP_TYPE_NOT_NULL(Bool , SQL_C_USHORT)
MAKE_BIND_COL_ODBCWRAP_TYPE_NOT_NULL(Int16, SQL_C_SSHORT)
MAKE_BIND_COL_ODBCWRAP_TYPE_NOT_NULL(UInt16 , SQL_C_USHORT)
MAKE_BIND_COL_ODBCWRAP_TYPE_NOT_NULL(Int32, SQL_C_SLONG)
MAKE_BIND_COL_ODBCWRAP_TYPE_NOT_NULL(UInt32, SQL_C_ULONG)
MAKE_BIND_COL_ODBCWRAP_TYPE_NOT_NULL(Int64, SQL_C_SBIGINT)
MAKE_BIND_COL_ODBCWRAP_TYPE_NOT_NULL(UInt64, SQL_C_UBIGINT)
MAKE_BIND_COL_ODBCWRAP_TYPE_NOT_NULL(Float, SQL_C_FLOAT)
MAKE_BIND_COL_ODBCWRAP_TYPE_NOT_NULL(Double, SQL_C_DOUBLE)
#undef MAKE_BIND_COL_ODBCWRAP_TYPE_NOT_NULL

#define MAKE_BIND_COL_ODBCWRAP_TYPE_NULLABLE(type, sql_type) \
void \
bindCol (const int index, type * buffer) { \
    buffer->len = 0; \
    SQLRETURN status = SQLBindCol(stmt->stmt, index, sql_type, &buffer->value, 0, &buffer->len); \
    if (status != SQL_SUCCESS) { \
        throw odbc_error(SQL_HANDLE_STMT, stmt->stmt, status); \
    } \
}
MAKE_BIND_COL_ODBCWRAP_TYPE_NULLABLE(NullInt16, SQL_C_SSHORT)
MAKE_BIND_COL_ODBCWRAP_TYPE_NULLABLE(NullUInt16 , SQL_C_USHORT)
MAKE_BIND_COL_ODBCWRAP_TYPE_NULLABLE(NullInt32, SQL_C_SLONG)
MAKE_BIND_COL_ODBCWRAP_TYPE_NULLABLE(NullUInt32, SQL_C_ULONG)
MAKE_BIND_COL_ODBCWRAP_TYPE_NULLABLE(NullInt64, SQL_C_SBIGINT)
MAKE_BIND_COL_ODBCWRAP_TYPE_NULLABLE(NullUInt64, SQL_C_UBIGINT)
MAKE_BIND_COL_ODBCWRAP_TYPE_NULLABLE(NullFloat, SQL_C_FLOAT)
MAKE_BIND_COL_ODBCWRAP_TYPE_NULLABLE(NullDouble, SQL_C_DOUBLE)
#undef MAKE_BIND_COL_ODBCWRAP_TYPE_NULLABLE

        template <int N>
        void
        bindCol(const int index, Char<N> * buffer) {
            buffer->len = 0;
            SQLRETURN status = SQLBindCol(stmt->stmt, index, SQL_C_CHAR, buffer->value, N, &buffer->len);
            if (status != SQL_SUCCESS){
                throw odbc_error(SQL_HANDLE_STMT, stmt->stmt, status);
            }
        }
        template <int N>
        void
        bindCol(const int index, NullChar<N> * buffer) {
            buffer->len = 0;
            SQLRETURN status = SQLBindCol(stmt->stmt, index, SQL_C_CHAR, buffer->value, N, &buffer->len);
            if (status != SQL_SUCCESS){
                throw odbc_error(SQL_HANDLE_STMT, stmt->stmt, status);
            }
        }


        template< unsigned N >
        void bindCol(const int index, const char (&arr)[N] ) /* throwable */ {
            SQLRETURN status = SQLBindCol(stmt->stmt, index, SQL_C_CHAR, arr, N, 0);
            if (status != SQL_SUCCESS){
                throw odbc_error(SQL_HANDLE_STMT, stmt->stmt, status);
            }
        }

        bool
        fetch() {
            SQLRETURN status = SQL_SUCCESS;
            SQLSMALLINT col = 0;

            status = SQLNumResultCols(stmt->stmt, &col);
            if (status != SQL_SUCCESS){
                throw odbc_error(SQL_HANDLE_STMT, stmt->stmt, status);
            }

            status = SQLFetch(stmt->stmt);
            if (status != SQL_SUCCESS && status != SQL_SUCCESS_WITH_INFO) {
                if (status == SQL_NULL_DATA) {
                    throw odbc_error("failure SQLFetch SQL_NULL_DATA");
                }

                status = SQLFreeStmt(stmt->stmt, SQL_CLOSE);
                return false;
            }
            return true;
        }
        long  updateRowCount() { return rowCount(); }
        long  insertRowCount() { return rowCount(); }
        long  deleteRowCount() { return rowCount(); }
        long
        rowCount() {
            SQLRETURN status = SQL_SUCCESS;
            SQLLEN row = 0;
            status = SQLRowCount(stmt->stmt, &row);
            if( status != SQL_SUCCESS){
                throw odbc_error(SQL_HANDLE_STMT, stmt->stmt, status);
            }

            status = SQLFreeStmt(stmt->stmt, SQL_CLOSE);
            return row;
        }
    };
}

namespace odbcwrap {
    class odbc_connection {
    public:
        std::string dsn;
        std::string uid;
        std::string pwd; 
        /* TODO 만약 odbc_connection을 new로 사용한다면 odbc_hdbc를 shared_ptr로 두어야 하고, 
        /* odbc_connection을 shared_ptr로 사용한다면 굳이 odbc_hdbc를 shared_ptr로 하지 않아도 될것같은데..
         */
        std::shared_ptr<odbc_hdbc> dbc;
        
        static
        std::shared_ptr<odbc_connection>
        make() {
            return std::shared_ptr<odbc_connection>(new odbc_connection);
        }

    public:
        
        odbc_connection *
        init () /* throwable */ {
            auto om = odbc_env::getInstance();
            SQLRETURN status = SQL_SUCCESS;
            SQLHDBC tmpdbc;
            status = SQLAllocHandle(SQL_HANDLE_DBC, om->env, &tmpdbc);
            if (status != SQL_SUCCESS) {
                throw odbc_error("failure SQLAllocHandle SQL_HANDLE_DBC");
            }
            dbc = std::make_shared<odbc_hdbc>(tmpdbc);

            return this;
        }
    private:
        odbc_connection() = default;
        odbc_connection(odbc_connection && rhs) noexcept
            : dsn(std::move(rhs.dsn))
            , uid(std::move(rhs.uid))
            , pwd(std::move(rhs.pwd))
            , dbc(std::move(rhs.dbc))
        {}

    public:

        void
        setDsn(const std::string & dsn) noexcept {
            this->dsn = dsn;
        }

        void
        setUid(const std::string & uid) noexcept {
            this->uid = uid;
        }

        void
        setPwd(const std::string & pwd) noexcept {
            this->pwd = pwd;
        }

        void
        setAutoCommit(bool autocommit) /* throwable */ {
            SQLRETURN status = 
            SQLSetConnectAttr (dbc->dbc, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)autocommit, 0); 
            if (status != SQL_SUCCESS){
                throw odbc_error(SQL_HANDLE_DBC, dbc->dbc, status);
            }
        }

        bool
        getAutoCommit() /* throwable */ {
            SQLINTEGER value = 0;
            SQLRETURN status = 
            SQLGetConnectAttr(dbc->dbc, SQL_ATTR_AUTOCOMMIT, 
                (SQLPOINTER) &value, (SQLINTEGER) sizeof(value), NULL);
            if (status != SQL_SUCCESS){
                throw odbc_error(SQL_HANDLE_DBC, dbc->dbc, status);
            }
            return (bool)value; 
        }


        /*
        https://stackoverflow.com/questions/2309684/connection-timeout-in-odbc
        I had the same question. Actually, just the moment I'm writing this, 
        I checked the SQLSetConnectAttr and was surprisedly reading the first sentence

        "The SQL Server Native Client ODBC driver ignores the setting of SQL_ATTR_CONNECTION_TIMEOUT."
        커넥션 timeout은 안된다고 합니다..
        */
        void
        setConnectionTimeout(const long timeout) {
            SQLRETURN status = 
            SQLSetConnectAttr (dbc->dbc, 
            SQL_ATTR_CONNECTION_TIMEOUT, (SQLPOINTER) timeout, 0);
            if (status != SQL_SUCCESS){
                throw odbc_error(SQL_HANDLE_DBC, dbc->dbc, status);
            }
        }

        int
        getConnectionTimeout() {
            SQLINTEGER value = 0;
            SQLRETURN status = 
            SQLGetConnectAttr(dbc->dbc, SQL_ATTR_CONNECTION_TIMEOUT, 
                (SQLPOINTER) &value, (SQLINTEGER) sizeof(value), NULL);
            if (status != SQL_SUCCESS){
                throw odbc_error(SQL_HANDLE_DBC, dbc->dbc, status);
            }
            return value; 
        }


        void
        setLoginTimeout(const long login_timeout) {
            SQLRETURN status = 
            SQLSetConnectAttr (dbc->dbc, 
            SQL_ATTR_LOGIN_TIMEOUT, (SQLPOINTER) login_timeout, 0);
            if (status != SQL_SUCCESS){
                throw odbc_error(SQL_HANDLE_DBC, dbc->dbc, status);
            }
        }

        long
        getLoginTimeout() /* throwable */ {
            SQLINTEGER value;
            SQLRETURN status = 
            SQLGetConnectAttr(dbc->dbc, SQL_ATTR_LOGIN_TIMEOUT, 
                (SQLPOINTER) &value, (SQLINTEGER) sizeof(value), NULL);
            if (status != SQL_SUCCESS){
                throw odbc_error(SQL_HANDLE_DBC, dbc->dbc, status);
            }
            return value;
        }


        bool
        getConnectionDead() noexcept {
            SQLINTEGER value = true;
            SQLRETURN status = 
            SQLGetConnectAttr(dbc->dbc, SQL_ATTR_CONNECTION_DEAD, 
                (SQLPOINTER) &value, 0, NULL);
            if (status != SQL_SUCCESS){
                SQLDisconnect(dbc->dbc);
            }
            return (bool)value;
        }

        void
        disconnectDB() noexcept {
            SQLRETURN status = SQLDisconnect(dbc->dbc);
            if (status != SQL_SUCCESS){
                // 2021-06-06 noexcpt 
                // throw odbc_error(SQL_HANDLE_DBC, dbc->dbc, status);
            }
        }

        void
        connectDB() /* throwable */ {
            static std::mutex mtx;
            mtx.lock();

            std::string connstr = 
            (!dsn.empty() ? "DSN=" + dsn + ";" : dsn) + 
            (!uid.empty() ? "UID=" + uid + ";" : uid) + 
            (!pwd.empty() ? "PWD=" + pwd : pwd);
            SQLRETURN status = 
            SQLDriverConnect(dbc->dbc, NULL, (SQLCHAR *)connstr.c_str(), 
                connstr.length(), NULL, 0, NULL, SQL_DRIVER_COMPLETE );
            if (status != SQL_SUCCESS){
                mtx.unlock();
                throw odbc_error(SQL_HANDLE_DBC, dbc->dbc, status);
            }
            mtx.unlock();
        }

        void
        commit(){
            SQLRETURN status = SQLTransact(NULL, dbc->dbc, SQL_COMMIT);
            if (status != SQL_SUCCESS){
                throw odbc_error(SQL_HANDLE_DBC, dbc->dbc, status);
            }
        }

        void
        rollback(){
            SQLRETURN status = SQLTransact(NULL, dbc->dbc, SQL_ROLLBACK);
            if (status != SQL_SUCCESS){
                throw odbc_error(SQL_HANDLE_DBC, dbc->dbc, status);
            }
        }

        std::shared_ptr<odbc_statement>
        sql() {
            return std::shared_ptr<odbc_statement>(
                new odbc_statement(*dbc));
        }
    };
}



namespace odbcwrap {
    
    class odbc_connnection_pool {
    public:
        // static
        // tp::MPMCBoundedQueue<std::shared_ptr<odbc_connection>>
        // make(size_t size) {
        //     return pool;
        // }

    };
}