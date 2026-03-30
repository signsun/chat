#pragma once

#include "chat.h"

class HttpConnection : public std::enable_shared_from_this<HttpConnection>
{
    friend class LogicSystem;

public:
    HttpConnection(boost::asio::io_context &ioc);

    /**
     * @brief 监听读写事件
     *
     **/
    void Start();

    tcp::socket &GetSocket()
    {
        return _socket;
    }

private:
    /**
     * @brief 超时检测
     *
     **/
    void CheckDeadline();

    /**
     * @brief 响应函数
     *
     **/
    void WriteResponse();

    /**
     * @brief 处理请求
     *
     * @details 解析请求头，解析请求内容
     *
     **/
    void HandleReq();

    /**
     * @brief 参数解析
     *
     **/
    void PreParseGetParam();

private:
    tcp::socket _socket;
    beast::flat_buffer _buffer{8192};                         // 字节数组，接受对方发来的数据
    http::request<http::dynamic_body> _request;               // 接受请求
    http::response<http::dynamic_body> _response;             // 响应请求
    std::string _get_url;                                     // url
    std::unordered_map<std::string, std::string> _get_params; // 存储解析后的参数
    /**
     * @brief 定时器
     *
     * @details _socket.get_executor()调度器  std::chrono::seconds(60)超时时间60秒
     **/
    net::steady_timer deadline_{
        _socket.get_executor(), std::chrono::seconds(60)};
};