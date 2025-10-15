#pragma once
#include <grpcpp/grpcpp.h>
#include "message.grpc.pb.h"
#include "common.h"
#include "singleton.h"
#include <atomic>
#include <queue>
#include <condition_variable>
#include <mutex>
using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

using message::GetVarifyReq;
using message::GetVarifyRsp;
using message::VarifyService;

class RPConPool
{
public:
    RPConPool(size_t poolsize, std::string host, std::string port);
    ~RPConPool();
    void Close();
    std::unique_ptr<VarifyService::Stub> getConnection();
    void returnConnection(std::unique_ptr<VarifyService::Stub> context);

private:
    std::atomic<bool> _b_stop;
    size_t _poolsize;
    std::string _host;
    std::string _port;
    std::queue<std::unique_ptr<VarifyService::Stub>> _connections;
    std::condition_variable _cv;
    std::mutex _mtx;
};

class VarifyGrpcClient : public Singleton<VarifyGrpcClient>
{
    friend class Singleton<VarifyGrpcClient>;

public:
    GetVarifyRsp GetVarifyCode(const std::string &email)
    {
        ClientContext context;
        GetVarifyRsp reply;
        GetVarifyReq request;
        request.set_email(email);
        auto stub = _pool->getConnection();
        Status status = stub->GetVarifyCode(&context, request, &reply);
        if (status.ok())
        {
            _pool->returnConnection(std::move(stub));
            return reply;
        }
        else
        {
            _pool->returnConnection(std::move(stub));
            reply.set_error(ErrorCodes::RPCFailed);
            return reply;
        }
    }

private:
    VarifyGrpcClient();
    std::unique_ptr<RPConPool> _pool;
};