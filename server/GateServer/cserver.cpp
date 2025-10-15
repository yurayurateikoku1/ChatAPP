#include "cserver.h"
#include "asioioservicepool.h"
CServer::CServer(boost::asio::io_context &ioc, unsigned short &port)
    : _ioc(ioc),
      _acceptor(ioc, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
{
}
void CServer::Start()
{
    auto self(shared_from_this());
    auto &io_context = AsioIOServicePool::GetInstance()->GetIOService();
    std::shared_ptr<HttpConnection> new_con = std::make_shared<HttpConnection>(io_context);
    _acceptor.async_accept(new_con->GetSocket(), [self, new_con](boost::system::error_code ec)
                           {
            try {
                if (ec)
                {
                    self->Start();
                    return;
                }
                new_con->Start();
                self->Start();
            }
            catch(const std::exception& e){
            } });
}