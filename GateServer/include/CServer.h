/**
 * @file CServer.h
 * @author nmt
 * @brief GateServer 服务器
 * @version 1.0.0
 * @date 2025-10-30
 **/
#pragma once
#include "chat.h"

class CServer : public std::enable_shared_from_this<CServer>
{
public:
    /**
     * @brief Construct a new CServer object
     *
     * @param  ioc     监听上下文
     * @param  port    监听端口
     **/
    CServer(boost::asio::io_context &ioc, unsigned short &port);

    /**
     * @brief 启动服务器，开始监听
     *
     **/
    void Start();

private:
    tcp::acceptor _acceptor; // 接收器，接收对端链接
    net::io_context &_ioc;   // 上下文
    // tcp::socket _socket;
};
