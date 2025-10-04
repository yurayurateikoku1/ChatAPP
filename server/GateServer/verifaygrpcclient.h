#pragma once
#include <grpcpp/grpcpp.h>
#include "message.grpc.pb.h"
#include "common.h"
#include "singleton.h"
using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

using message::;
using message::GetVarifyReq;
using message::GetVarifyRsp;
using message::VarifyService;
class VerifayGrpcClient : public Singleton<VerifayGrpcClient>
{
    friend class Singleton<VerifayGrpcClient>;

public:
    GetVarifyRsp GetVarifyCode(const std::string &email)
    {
        ClientContext context;
        GetVarifyRsp reply;
        GetVarifyReq request;
        request.set_email(email);

        Status status_ = stub_->GetVarifyCode(&context, request, &reply);
        if (status.ok())
        {

            return reply;
        }
        else
        {
            reply.set_error(ErrorCodes::RPCFailed);
            return reply;
        }
    }

private:
    VerifyGrpcClient()
    {
        std::shared_ptr<Channel> channel = grpc::CreateChannel("127.0.0.1:50051", grpc::InsecureChannelCredentials());
        stub_ = VarifyService::NewStub(channel);
    }
    std::unique_ptr<VarifyService::Stub> stub_;
};