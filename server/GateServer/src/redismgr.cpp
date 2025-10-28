#include "redismgr.h"
#include "configmgr.h"
RedisMgr::RedisMgr()
{
    auto &gCfmgr = ConfigMgr::GetInstance();
    auto host = gCfmgr["Redis"]["Host"];
    auto port = gCfmgr["Redis"]["Port"];
    auto pwd = gCfmgr["Redis"]["Passwd"];
    _con_pool.reset(new RedisConPool(5, host.c_str(), atoi(port.c_str()), pwd.c_str()));
}

bool RedisMgr::Get(const std::string &key, std::string &value)
{
    auto connect = _con_pool->getConnection();
    if (connect == nullptr)
    {
        return false;
    }
    auto reply = static_cast<redisReply *>(redisCommand(connect, "GET %s", key.c_str()));
    if (reply == NULL)
    {
        std::cout << "GET[" << key << "] failed" << std::endl;
        freeReplyObject(reply);
        return false;
    }

    if (reply->type != REDIS_REPLY_STRING)
    {
        std::cout << "GET[" << key << "] failed" << std::endl;
        freeReplyObject(reply);
        return false;
    }

    value = reply->str;
    freeReplyObject(reply);
    std::cout << "Success to execute command GET[" << key << "] success" << std::endl;
    return true;
}

bool RedisMgr::Set(const std::string &key, const std::string &value)
{
    auto connect = _con_pool->getConnection();
    if (connect == nullptr)
    {
        return false;
    }
    auto reply = static_cast<redisReply *>(redisCommand(connect, "SET %s %s", key.c_str(), value.c_str()));
    if (reply == NULL)
    {
        std::cout << "SET[" << key << "] failed" << std::endl;
        freeReplyObject(reply);
        return false;
    }

    if (!(reply->type == REDIS_REPLY_STATUS && (strcmp(reply->str, "OK") == 0 || strcmp(reply->str, "ok") == 0)))
    {
        std::cout << "SET[" << key << "] failed" << std::endl;
        freeReplyObject(reply);
        return false;
    }

    freeReplyObject(reply);
    std::cout << "Success to execute command SET[" << key << "] success" << std::endl;
    return true;
}

bool RedisMgr::Auth(const std::string &password)
{
    auto connect = _con_pool->getConnection();
    if (connect == nullptr)
    {
        return false;
    }
    auto reply = static_cast<redisReply *>(redisCommand(connect, "AUTH %s", password.c_str()));
    if (reply->type == REDIS_REPLY_ERROR)
    {
        std::cout << "Redis authentication failed" << std::endl;
        freeReplyObject(reply);
        return false;
    }
    freeReplyObject(reply);
    std::cout << "Redis authentication success" << std::endl;
    return true;
}

bool RedisMgr::LPush(const std::string &key, const std::string &value)
{
    auto connect = _con_pool->getConnection();
    if (connect == nullptr)
    {
        return false;
    }
    auto reply = static_cast<redisReply *>(redisCommand(connect, "LPUSH %s %s", key.c_str(), value.c_str()));
    if (reply == NULL)
    {
        std::cout << "LPUSH[" << key << "] failed" << std::endl;
        freeReplyObject(reply);
        return false;
    }

    if (reply->type != REDIS_REPLY_INTEGER || reply->integer <= 0)
    {
        std::cout << "LPUSH[" << key << "] failed" << std::endl;
        freeReplyObject(reply);
        return false;
    }

    freeReplyObject(reply);
    std::cout << "Success to execute command LPUSH[" << key << "] success" << std::endl;
    return true;
}

bool RedisMgr::LPop(const std::string &key, std::string &value)
{
    auto connect = _con_pool->getConnection();
    if (connect == nullptr)
    {
        return false;
    }
    auto reply = static_cast<redisReply *>(redisCommand(connect, "LPOP %s", key.c_str()));
    if (reply == NULL || reply->type == REDIS_REPLY_NIL)
    {
        std::cout << "LPOP[" << key << "] failed" << std::endl;
        freeReplyObject(reply);
        return false;
    }
    value = reply->str;
    freeReplyObject(reply);
    std::cout << "Success to execute command LPOP[" << key << "] success" << std::endl;
    return true;
}

bool RedisMgr::RPush(const std::string &key, const std::string &value)
{
    auto connect = _con_pool->getConnection();
    if (connect == nullptr)
    {
        return false;
    }
    auto reply = static_cast<redisReply *>(redisCommand(connect, "RPUSH %s %s", key.c_str(), value.c_str()));
    if (reply == NULL)
    {
        std::cout << "RPUSH[" << key << "] failed" << std::endl;
        freeReplyObject(reply);
        return false;
    }

    if (reply->type != REDIS_REPLY_INTEGER || reply->integer <= 0)
    {
        std::cout << "RPUSH[" << key << "] failed" << std::endl;
        freeReplyObject(reply);
        return false;
    }

    freeReplyObject(reply);
    std::cout << "Success to execute command RPUSH[" << key << "] success" << std::endl;
    return true;
}

bool RedisMgr::RPop(const std::string &key, std::string &value)
{
    auto connect = _con_pool->getConnection();
    if (connect == nullptr)
    {
        return false;
    }
    auto reply = static_cast<redisReply *>(redisCommand(connect, "RPOP %s", key.c_str()));
    if (reply == NULL || reply->type == REDIS_REPLY_NIL)
    {
        std::cout << "RPOP[" << key << "] failed" << std::endl;
        freeReplyObject(reply);
        return false;
    }
    value = reply->str;
    freeReplyObject(reply);
    std::cout << "Success to execute command RPOP[" << key << "] success" << std::endl;
    return true;
}

bool RedisMgr::HSet(const std::string &key, const std::string &hkey, const std::string &value)
{
    auto connect = _con_pool->getConnection();
    if (connect == nullptr)
    {
        return false;
    }
    auto reply = static_cast<redisReply *>(redisCommand(connect, "HSET %s %s %s", key.c_str(), hkey.c_str(), value.c_str()));
    if (reply == NULL || reply->type != REDIS_REPLY_INTEGER)
    {
        std::cout << "HSET[" << key << " " << hkey << "] failed" << std::endl;
        freeReplyObject(reply);
        return false;
    }
    std::cout << "Success to execute command HSET[" << key << " " << hkey << "] success" << std::endl;
    freeReplyObject(reply);
    return true;
}

bool RedisMgr::HSet(const char *key, const char *hkey, const char *hvalue, size_t hvaluelen)
{
    auto connect = _con_pool->getConnection();
    if (connect == nullptr)
    {
        return false;
    }
    const char *argv[4];
    size_t argvlen[4];
    argv[0] = "HSET";
    argvlen[0] = 4;
    argv[1] = key;
    argvlen[1] = strlen(key);
    argv[2] = hkey;
    argvlen[2] = strlen(hkey);
    argv[3] = hvalue;
    argvlen[3] = hvaluelen;

    auto reply = static_cast<redisReply *>(redisCommandArgv(connect, 4, argv, argvlen));
    if (reply == NULL || reply->type != REDIS_REPLY_INTEGER)
    {
        std::cout << "HSET[" << key << " " << hkey << "] failed" << std::endl;
        freeReplyObject(reply);
        return false;
    }
    std::cout << "Success to execute command HSET[" << key << " " << hkey << "] success" << std::endl;
    freeReplyObject(reply);
    return true;
}

std::string RedisMgr::HGet(const std::string &key, const std::string &hkey)
{
    auto connect = _con_pool->getConnection();
    if (connect == nullptr)
    {
        return "";
    }
    const char *argv[3];
    size_t argvlen[3];
    argv[0] = "HGET";
    argvlen[0] = 4;
    argv[1] = key.c_str();
    argvlen[1] = key.length();
    argv[2] = hkey.c_str();
    argvlen[2] = hkey.length();
    auto reply = static_cast<redisReply *>(redisCommandArgv(connect, 3, argv, argvlen));
    if (reply == NULL || reply->type == REDIS_REPLY_NIL)
    {
        std::cout << "HGET[" << key << " " << hkey << "] failed" << std::endl;
        freeReplyObject(reply);
        return "";
    }

    std::string value = reply->str;
    freeReplyObject(reply);
    std::cout << "Success to execute command HGET[" << key << " " << hkey << "] success" << std::endl;
    return value;
}

bool RedisMgr::Del(const std::string &key)
{
    auto connect = _con_pool->getConnection();
    if (connect == nullptr)
    {
        return false;
    }
    auto reply = static_cast<redisReply *>(redisCommand(connect, "DEL %s", key.c_str()));
    if (reply == NULL || reply->type != REDIS_REPLY_INTEGER)
    {
        std::cout << "DEL[" << key << "] failed" << std::endl;
        freeReplyObject(reply);
        return false;
    }

    freeReplyObject(reply);
    std::cout << "Success to execute command DEL[" << key << "] success" << std::endl;
    return true;
}

bool RedisMgr::ExistsKey(const std::string &key)
{
    auto connect = _con_pool->getConnection();
    if (connect == nullptr)
    {
        return false;
    }
    auto reply = static_cast<redisReply *>(redisCommand(connect, "EXISTS %s", key.c_str()));
    if (reply == NULL || reply->type != REDIS_REPLY_INTEGER || reply->integer == 0)
    {
        std::cout << "EXISTS[" << key << "] failed" << std::endl;
        freeReplyObject(reply);
        return false;
    }

    freeReplyObject(reply);
    std::cout << "Success to execute command EXISTS[" << key << "] success" << std::endl;
    return true;
}

void RedisMgr::Close()
{
    _con_pool->Close();
}

RedisMgr::~RedisMgr()
{
    Close();
}

RedisConPool::RedisConPool(size_t poolsize, const char *host, int port, const char *password)
    : _poolSize(poolsize), _host(host), _port(port), _b_stop(false)
{
    for (size_t i = 0; i < _poolSize; ++i)
    {
        auto *context = redisConnect(_host, _port);
        if (context == nullptr || context->err != 0)
        {
            if (context != nullptr)
            {
                redisFree(context);
            }
            continue;
        }
        auto *reply = static_cast<redisReply *>(redisCommand(context, "AUTH %s", password));
        if (reply->type == REDIS_REPLY_ERROR)
        {
            std::cout << "Redis authentication failed" << std::endl;
            freeReplyObject(reply);
            redisFree(context);
            continue;
        }
        freeReplyObject(reply);
        std::cout << "Redis authentication success" << std::endl;
        _connections.push(context);
    }
}
RedisConPool::~RedisConPool()
{

    std::lock_guard<std::mutex> lock(_mutex);
    while (!_connections.empty())
    {
        _connections.pop();
    }
}

redisContext *RedisConPool::getConnection()
{
    std::unique_lock<std::mutex> lock(_mutex);
    _cv.wait(lock, [this]()
             { 
                if (_b_stop){
                    return true;
                }
                return !_connections.empty(); });
    if (_b_stop)
    {
        return nullptr;
    }
    auto *context = _connections.front();
    _connections.pop();
    return context;
}

void RedisConPool::returnConnection(redisContext *context)
{
    std::lock_guard<std::mutex> lock(_mutex);
    if (_b_stop)
    {
        return;
    }
    _connections.push(context);
    _cv.notify_one();
}

void RedisConPool::Close()
{
    _b_stop = true;
    _cv.notify_all();
}