#include "httpconnection.h"
HttpConnection::HttpConnection(boost::asio::ip::tcp::socket socket)
    : _socket(std::move(socket))
{
}

void HttpConnection::Start()
{
    auto self = shared_from_this();
    boost::beast::http::async_read(_socket, _buffer, _request, [self](boost::beast::error_code ec, std::size_t bytes_transferred)
                                   {
            try {
                if (ec) {
                    std::cout << "http read err is " << ec.what() << std::endl;
                    return;
                }

                //处理读到的数据
                boost::ignore_unused(bytes_transferred);
                self->HandleRequest();
                self->CheckDeadline();
            }
            catch (std::exception& exp) {
                std::cout << "exception is " << exp.what() << std::endl;
            } });
}

void HttpConnection::HandleRequest()
{
    // 设置版本
    _response.version(_request.version());
    // 设置为短链接
    _response.keep_alive(false);

    if (_request.method() == boost::beast::http::verb::get)
    {
        bool success = LogicSystem::GetInstance()->HandleGet(_request.target(), shared_from_this());
        if (!success)
        {
            _response.result(boost::beast::http::status::not_found);
            _response.set(boost::beast::http::field::content_type, "text/plain");
            boost::beast::ostream(_response.body()) << "url not found\r\n";
            WriteResponse();
            return;
        }

        _response.result(boost::beast::http::status::ok);
        _response.set(boost::beast::http::field::server, "GateServer");
        WriteResponse();
        return;
    }
}

void HttpConnection::WriteResponse()
{
    auto self = shared_from_this();
    _response.prepare_payload();
    boost::beast::http::async_write(_socket, _response, [self](boost::beast::error_code ec, std::size_t)
                                    {
            try {
                self->_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_send, ec);
                //连接关闭
                self->_deadline.cancel();
            }
            catch (std::exception& exp) {
                std::cout << "exception is " << exp.what() << std::endl;
            } });
}