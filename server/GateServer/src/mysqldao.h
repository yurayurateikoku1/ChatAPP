#pragma once
#include "common.h"
#include <thread>
#include <jdbc/mysql_driver.h>
#include <jdbc/mysql_connection.h>
#include <jdbc/cppconn/prepared_statement.h>
#include <jdbc/cppconn/resultset.h>
#include <jdbc/cppconn/statement.h>
#include <jdbc/cppconn/exception.h>
class SqlConnection
{
public:
    SqlConnection(sql::Connection *conn, int64_t lasttime);
    std::unique_ptr<sql::Connection> _conn;
    int64_t _last_oper_time;
};

class MySqlPool
{
public:
    MySqlPool(const std::string &url, const std::string &user, const std::string &password, const std::string &schema, int poolSize);
    ~MySqlPool();

    std::unique_ptr<SqlConnection> getConnection();
    void returnConnection(std::unique_ptr<SqlConnection> con);
    void Close();

private:
    void checkConnections();

    std::string _url;
    std::string _user;
    std::string _password;
    std::string _schema;
    int _pool_size;

    std::queue<std::unique_ptr<SqlConnection>> _pool;
    std::mutex _mutex;
    std::condition_variable _cv;
    std::atomic<bool> _b_stop;
    std::thread _checkThread;
};

struct UserInfo
{
    std::string name;
    std::string pwd;
    int uid;
    std::string email;
};

class MysqlDao
{
public:
    MysqlDao();
    ~MysqlDao();
    int RegUser(const std::string &name, const std::string &email, const std::string &password);
    bool CheckEmail(const std::string &name, const std::string &email);
    bool UpdatePwd(const std::string &name, const std::string &newpwd);
    bool CheckPwd(const std::string &name, const std::string &pwd, UserInfo &userInfo);

private:
    std::unique_ptr<MySqlPool> _pool;
};