#pragma once
#include "common.h"
#include <hiredis/hiredis.h>
#include "singleton.h"

class RedisConPool
{
public:
    RedisConPool(size_t poolsize, const char *host, int port, const char *password);
    ~RedisConPool();
    redisContext *getConnection();
    void returnConnection(redisContext *conn);
    void Close();

private:
    std::atomic<bool> _b_stop;
    size_t _poolSize;
    const char *_host;
    int _port;
    std::queue<redisContext *> _connections;
    std::mutex _mutex;
    std::condition_variable _cv;
};

class RedisMgr : public Singleton<RedisMgr>,
                 public std::enable_shared_from_this<RedisMgr>
{
    friend class Singleton<RedisMgr>;

public:
    ~RedisMgr();
    bool Get(const std::string &key, std::string &value);
    bool Set(const std::string &key, const std::string &value);
    bool Auth(const std::string &password);
    bool LPush(const std::string &key, const std::string &value);
    bool LPop(const std::string &key, std::string &value);
    bool RPush(const std::string &key, const std::string &value);
    bool RPop(const std::string &key, std::string &value);
    bool HSet(const std::string &key, const std::string &hkey, const std::string &value);
    bool HSet(const char *key, const char *hkey, const char *hvalue, size_t hvaluelen);
    std::string HGet(const std::string &key, const std::string &hkey);
    bool Del(const std::string &key);
    bool ExistsKey(const std::string &key);
    void Close();

private:
    RedisMgr();
    std::unique_ptr<RedisConPool> _con_pool;
};
