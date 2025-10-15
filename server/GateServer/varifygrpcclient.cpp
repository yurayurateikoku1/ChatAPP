#include "varifygrpcclient.h"
#include "configmgr.h"
RPConPool::RPConPool(size_t poolsize, std::string host, std::string port)
    : _poolsize(poolsize), _host(host), _port(port), _b_stop(false)
{
    for (size_t i = 0; i < _poolsize; ++i)
    {
        auto channel = grpc::CreateChannel(host + ":" + port, grpc::InsecureChannelCredentials());
        _connections.push(VarifyService::NewStub(channel));
    }
}

RPConPool::~RPConPool()
{
    std::lock_guard<std::mutex> lock(_mtx);
    Close();
    while (!_connections.empty())
    {
        _connections.pop();
    }
}

void RPConPool::Close()
{
    _b_stop.store(true);
    _cv.notify_all();
}

std::unique_ptr<VarifyService::Stub> RPConPool::getConnection()
{
    std::unique_lock<std::mutex> lock(_mtx);
    _cv.wait(lock, [this]()
             { 
                if(_b_stop){
                    return true;
                } 
                return !_connections.empty(); });
    if (_b_stop)
    {
        return nullptr;
    }
    auto conn = std::move(_connections.front());
    _connections.pop();
    return conn;
}

void RPConPool::returnConnection(std::unique_ptr<VarifyService::Stub> context)
{

    std::lock_guard<std::mutex> lock(_mtx);
    if (_b_stop)
    {
        return;
    }
    _connections.push(std::move(context));
    _cv.notify_one();
}

VarifyGrpcClient::VarifyGrpcClient()
{
    auto &gCfgMgr = ConfigMgr::GetInstance();
    std::string host = gCfgMgr["VarifyServer"]["Host"];
    std::string port = gCfgMgr["VarifyServer"]["Port"];
    _pool.reset(new RPConPool(5, host, port));
}