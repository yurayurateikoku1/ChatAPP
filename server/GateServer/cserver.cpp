#include "cserver.h"
CServer::CServer(boost::asio::io_context &ioc, unsigned short &port)
    : _ioc(ioc),
      _acceptor(ioc, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)), _socket(ioc)
{
}
void CServer::Start()
{
    auto self(shared_from_this());
    _acceptor.async_accept(_socket, [this, self](boost::system::error_code ec)
                           {
            try {
                if (!ec)
                {
                    self->Start();
                    return;
                }
                std::make_shared<HttpConnection>(std::move(_socket))->Start();
                self->Start();
            }
            catch(const std::exception& e){
            } });
}