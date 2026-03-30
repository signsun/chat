#pragma once

#include <grpcpp/grpcpp.h>

#include "message.grpc.pb.h"
#include "chat.h"
#include "Singleton.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

using message::GetVarifyReq;
using message::GetVarifyRsp;
using message::VarifyService;

class RPConpool
{
public:
    RPConpool(size_t poolsize, std::string host, std::string port);
    ~RPConpool();
    void Close();
    std::unique_ptr<VarifyService::Stub> getConnection();
    void returnConnection(std::unique_ptr<VarifyService::Stub> context);

private:
    std::atomic<bool> b_stop_;
    size_t poolSize_;
    std::string host_;
    std::string port_;
    std::queue<std::unique_ptr<VarifyService::Stub>> connections_;
    std::condition_variable cond_;
    std::mutex mutex_;
};

class VerifyGrpcClient : public Singleton<VerifyGrpcClient>
{
    friend class Singleton<VerifyGrpcClient>;

public:
    GetVarifyRsp GetVerifyCode(std::string email);

private:
    VerifyGrpcClient();

private:
    std::unique_ptr<RPConpool> pool_;
};
