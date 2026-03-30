#pragma once

#include "chat.h"

class HttpConnection;

typedef std::function<void(std::shared_ptr<HttpConnection>)> HttpHandler;

class LogicSystem : public Singleton<LogicSystem>
{
    friend class Singleton<LogicSystem>;

public:
    ~LogicSystem() {}

    /**
     * @brief 处理get请求
     *
     * @param url    url
     * @param connection HttpConnection的智能指针
     *
     * @return true
     * @return false
     **/
    bool HandleGet(std::string url, std::shared_ptr<HttpConnection> connection);

    /**
     * @brief
     *
     * @param  url         url
     * @param  connection  HttpConnection的智能指针
     * @return true
     * @return false
     **/
    bool HandlePost(std::string url, std::shared_ptr<HttpConnection> connection);

    /**
     * @brief 注册get请求
     *
     * @param url url
     * @param handler 处理器（回调函数）
     *
     **/
    void RegGet(std::string url, HttpHandler handler);

    /**
     * @brief 注册post请求
     *
     * @param  url        url
     * @param  handler    处理器（回调函数）
     **/
    void RegPost(std::string url, HttpHandler handler);

private:
    LogicSystem();

private:
    std::map<std::string, HttpHandler> _post_handlers; // 处理post请求的集合
    std::map<std::string, HttpHandler> _get_handlers;  // 处理get请求的集合
};