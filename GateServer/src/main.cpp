#include "CServer.h"
#include "ConfigMgr.h"
#include <hiredis/hiredis.h>
#include "RedisMgr.h"

int main()
{
    auto &gCfgMgr = ConfigMgr::Inst();
    std::string gate_port_str = gCfgMgr["GateServer"]["Port"];
    unsigned short gate_port = stoi(gate_port_str.c_str());
    try
    {
        unsigned short port = static_cast<unsigned short>(gate_port);
        net::io_context ioc{1};
        boost::asio::signal_set signals(ioc, SIGINT, SIGTERM);
        signals.async_wait([&ioc](const boost::system::error_code &error, int signal_number)
                           {
            if(error)
            {
                return;
            }

            ioc.stop(); });

        std::make_shared<CServer>(ioc, port)->Start();
        std::cout << "Gate Server listen on port: " << port << std::endl;
        ioc.run();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error:" << e.what() << '\n';
        return EXIT_FAILURE;
    }
}