#include "mysqldao.h"
#include "configmgr.h"

SqlConnection::SqlConnection(sql::Connection *conn, int64_t lasttime)
    : _conn(conn), _last_oper_time(lasttime)
{
}

MySqlPool::MySqlPool(const std::string &url, const std::string &user, const std::string &password, const std::string &schema, int poolSize)
    : _url(url), _user(user), _password(password), _schema(schema), _pool_size(poolSize), _b_stop(false)
{
    try
    {

        for (int i = 0; i < _pool_size; ++i)
        {
            sql::mysql::MySQL_Driver *driver = sql::mysql::get_mysql_driver_instance();
            auto *conn = driver->connect(_url, _user, _password);
            conn->setSchema(_schema);

            auto currentTime = std::chrono::system_clock::now().time_since_epoch();
            long long timestamp = std::chrono::duration_cast<std::chrono::seconds>(currentTime).count();
            _pool.push(std::make_unique<SqlConnection>(conn, timestamp));
        }

        _checkThread = std::thread([this]()
                                   { while(!_b_stop){
                                        checkConnections();
                                        std::this_thread::sleep_for(std::chrono::seconds(60));
                                     } });
        _checkThread.detach();
    }
    catch (sql::SQLException &e)
    {
        std::cerr << "MySqlPool initialization error: " << e.what() << std::endl;
        throw;
    }
}

std::unique_ptr<SqlConnection> MySqlPool::getConnection()
{
    std::unique_lock<std::mutex> lock(_mutex);
    _cv.wait(lock, [this]()
             { 
                if(_b_stop){
                    return true;
                }
                return !_pool.empty(); });
    if (_b_stop)
    {
        return nullptr;
    }

    std::unique_ptr<SqlConnection> con(std::move(_pool.front()));
    _pool.pop();
    return con;
}

void MySqlPool::returnConnection(std::unique_ptr<SqlConnection> con)
{
    std::unique_lock<std::mutex> lock(_mutex);
    if (!_b_stop)
    {
        return;
    }
    _pool.push(std::move(con));
    _cv.notify_one();
}

void MySqlPool::Close()
{
    _b_stop = true;
    _cv.notify_all();
}

void MySqlPool::checkConnections()
{
    std::lock_guard<std::mutex> guard(_mutex);
    int poolSize = _pool.size();
    auto currentTime = std::chrono::system_clock::now().time_since_epoch();
    long long timestamp = std::chrono::duration_cast<std::chrono::seconds>(currentTime).count();
    for (int i = 0; i < poolSize; i++)
    {
        auto con = std::move(_pool.front());
        _pool.pop();
        Defer defer([this, &con]()
                    { _pool.push(std::move(con)); });
        if (timestamp - con->_last_oper_time < 5)
        {
            continue;
        }
        try
        {
            std::unique_ptr<sql::Statement> stmt(con->_conn->createStatement());
            stmt->executeQuery("SELECT 1");
            con->_last_oper_time = timestamp;
            std::cout << "execute time alive query,cur is" << timestamp << std::endl;
        }
        catch (sql::SQLException &ex)
        {
            std::cout << "Error keeping connection alive:" << ex.what() << std::endl;
            sql::mysql::MySQL_Driver *driver = sql::mysql::get_driver_instance();
            auto *newcon = driver->connect(_url, _user, _password);
            newcon->setSchema(_schema);
            con->_conn.reset(newcon);
            con->_last_oper_time = timestamp;
        }
    }
}

MySqlPool::~MySqlPool()
{
    std::unique_lock<std::mutex> lock(_mutex);
    while (!_pool.empty())
    {
        _pool.pop();
    }
}

MysqlDao::MysqlDao()
{
    auto &cfg = ConfigMgr::GetInstance();
    const auto &host = cfg["MySQL"]["Host"];
    const auto &user = cfg["MySQL"]["User"];
    const auto &password = cfg["MySQL"]["Passwd"];
    const auto &schema = cfg["MySQL"]["Schema"];
    const auto &port = cfg["MySQL"]["Port"];
    _pool.reset(new MySqlPool(host + ":" + port, user, password, schema, 5));
}

MysqlDao::~MysqlDao()
{
    _pool->Close();
}

int MysqlDao::RegUser(const std::string &name, const std::string &email, const std::string &password)
{
    auto con = _pool->getConnection();
    try
    {
        if (con == nullptr)
        {
            return false;
        }
        std::unique_ptr<sql::PreparedStatement> stmt(
            con->_conn->prepareStatement("CALL reg_user(?,?,?,@result)"));
        stmt->setString(1, name);
        stmt->setString(2, email);
        stmt->setString(3, password);
        stmt->execute();

        std::unique_ptr<sql::Statement> stmtResult(
            con->_conn->createStatement());
        std::unique_ptr<sql::ResultSet> res(stmtResult->executeQuery("SELECT @result AS result"));

        if (res->next())
        {
            int result = res->getInt("result");
            std::cout << "Result:" << result << std::endl;
            _pool->returnConnection(std::move(con));
            return result;
        }

        _pool->returnConnection(std::move(con));
        return -1;
    }
    catch (sql::SQLException &e)
    {
        _pool->returnConnection(std::move(con));
        std::cerr << "RegUser error: " << e.what();
        std::cerr << " (MySQL error code: " << e.getErrorCode();
        std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
        return -1;
    }
}