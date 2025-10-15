#include "cserver.h"
#include <json/json.h>
#include <json/value.h>
#include <json/reader.h>
#include <iostream>
#include "configmgr.h"
int main()
{

    auto &gCfgMgr = ConfigMgr::GetInstance();
    std::string gate_port_str = gCfgMgr["GateServer"]["Port"];
    unsigned short gate_port = atoi(gate_port_str.c_str());
    try
    {
        unsigned short port = static_cast<unsigned short>(gate_port);
        boost::asio::io_context ioc{1};
        boost::asio::signal_set signals(ioc, SIGINT, SIGTERM);
        signals.async_wait([&ioc](const boost::system::error_code &ec, int signal_number)
                           {
                               if (ec)
                               {
                                return;
                               }
                               ioc.stop(); });
        std::make_shared<CServer>(ioc, port)->Start();
        std::cout << " GateServer start at port " << port << std::endl;
        ioc.run();
    }
    catch (std::exception &exp)
    {
        std::cout << "exception is " << exp.what() << std::endl;
        return EXIT_FAILURE;
    }
}
