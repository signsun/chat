#pragma once

#include "chat.h"
#include <hiredis/hiredis.h>

class RedisConPool
{
public:
    RedisConPool(size_t poolSize, const char *host, int port, const char *pwd);
    ~RedisConPool();
    redisContext *getConnection();
    void returnConnection(redisContext *context);
    void close();

private:
    std::atomic<bool> b_stop_;
    size_t poolSize_;
    const char *host_;
    int port_;
    std::queue<redisContext *> connections_;
    std::mutex mutex_;
    std::condition_variable cond_;
};

class RedisMgr : public Singleton<RedisMgr>
{
    friend class Singleton<RedisMgr>;

public:
    ~RedisMgr();
    // bool Connect(const std::string &host, int port);
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

private:
    std::unique_ptr<RedisConPool> _con_pool;
};

class RedisReplyGuard
{
private:
    redisReply *_reply;

public:
    RedisReplyGuard(redisReply *reply) : _reply(reply) {}
    ~RedisReplyGuard()
    {
        if (_reply)
        {
            freeReplyObject(_reply);
        }
    }
    redisReply *get() const { return _reply; }
};