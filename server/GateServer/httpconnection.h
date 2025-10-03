#pragma once
#include "common.h"
#include "logicsystem.h"
class HttpConnection : public std::enable_shared_from_this<HttpConnection>
{
public:
    friend class LogicSystem;
    HttpConnection(boost::asio::ip::tcp::socket socket);
    void Start();

private:
    void CheckDeadline();
    void WriteResponse();
    void HandleRequest();
    boost::asio::ip::tcp::socket _socket;
    boost::beast::flat_buffer _buffer{8192};
    boost::beast::http::request<boost::beast::http::dynamic_body> _request;
    boost::beast::http::response<boost::beast::http::dynamic_body> _response;
    boost::asio::steady_timer _deadline{_socket.get_executor(), std::chrono::seconds(60)};
};
