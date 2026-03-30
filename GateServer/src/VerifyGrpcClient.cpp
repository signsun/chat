#include "VerifyGrpcClient.h"
#include "ConfigMgr.h"

RPConpool::RPConpool(size_t poolsize, std::string host, std::string port) : poolSize_(poolsize),
                                                                            host_(host), port_(port), b_stop_(false)
{
    for (size_t i = 0; i < poolSize_; ++i)
    {
        std::shared_ptr<Channel> channel = grpc::CreateChannel(host + ":" + port, grpc::InsecureChannelCredentials());
        connections_.push(VarifyService::NewStub(channel));
    }
}

RPConpool::~RPConpool()
{
    std::lock_guard<std::mutex> lock(mutex_);
    Close();
    while (!connections_.empty())
    {
        connections_.pop();
    }
}

void RPConpool::Close()
{
    b_stop_ = true;
    cond_.notify_all();
}

std::unique_ptr<VarifyService::Stub> RPConpool::getConnection()
{
    std::unique_lock<std::mutex> lock(mutex_);
    cond_.wait(lock, [this]()
               {
        if(b_stop_)
        {
            return true;
        }
        return !connections_.empty(); });

    if (b_stop_)
    {
        return nullptr;
    }
    auto context = std::move(connections_.front());
    connections_.pop();
    return context;
}

void RPConpool::returnConnection(std::unique_ptr<VarifyService::Stub> context)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (b_stop_)
    {
        return;
    }

    connections_.push(std::move(context));
    cond_.notify_one();
}

VerifyGrpcClient::VerifyGrpcClient()
{
    auto &gCfgMgr = ConfigMgr::Inst();
    std::string host = gCfgMgr["VarifyServer"]["Host"];
    std::string port = gCfgMgr["VarifyServer"]["Port"];
    pool_.reset(new RPConpool(5, host, port));
}

GetVarifyRsp VerifyGrpcClient::GetVerifyCode(std::string email)
{
    ClientContext context;
    GetVarifyRsp reply;
    GetVarifyReq request;

    request.set_email(email);

    auto stub_ = pool_->getConnection();
    Status status = stub_->GetVarifyCode(&context, request, &reply);
    if (status.ok())
    {
        pool_->returnConnection(std::move(stub_));
        return reply;
    }
    else
    {
        pool_->returnConnection(std::move(stub_));
        reply.set_error(ErrorCodes::RPCFailed);
        return reply;
    }
}