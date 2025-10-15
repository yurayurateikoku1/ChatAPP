#include "logicsystem.h"
#include "httpconnection.h"
#include "varifygrpcclient.h"
void LogicSystem::RegGet(std::string url, HttpHandler handler)
{
    _get_handlers[url] = handler;
}

void LogicSystem::RegPost(std::string url, HttpHandler handler)
{
    _post_handlers[url] = handler;
}

LogicSystem::LogicSystem()
{
    RegGet("/get_test", [](std::shared_ptr<HttpConnection> conn)
           {
               boost::beast::ostream(conn->_response.body()) << "receive get test\r\n";
               int i = 0;
               for (auto &elem : conn->_get_params)
               {
                   i++;
                   boost::beast::ostream(conn->_response.body()) << "param" << i << " key is " << elem.first;
                   boost::beast::ostream(conn->_response.body()) << ", " << " value is " << elem.second << std::endl;
               } });
    RegPost("/get_varifycode", [](std::shared_ptr<HttpConnection> conn)
            {
                auto body_str = boost::beast::buffers_to_string(conn->_request.body().data());
                conn->_response.set(boost::beast::http::field::content_type, "text/json");
                Json::Value root;
                Json::Reader reader;
                Json::Value src_root;
                bool parse_success = reader.parse(body_str, src_root);
                if (!parse_success)
                {
                    root["error"] = Error_Json;
                    boost::beast::ostream(conn->_response.body()) << root.toStyledString();
                    return true;
                }
                if(!src_root.isMember("email")){
                    root["error"] = Error_Json;
                    boost::beast::ostream(conn->_response.body()) << root.toStyledString();
                    return true;
                }

                    auto email = src_root["email"].asString();
                    GetVarifyRsp rsp=VarifyGrpcClient::GetInstance()->GetVarifyCode(email); 
                    root["error"] = rsp.error();
                    root["email"] =src_root["email"];
                     std::string jsonstr= root.toStyledString();
                     boost::beast::ostream(conn->_response.body()) << jsonstr;
                     return true; });
}

LogicSystem::~LogicSystem()
{
}

bool LogicSystem::HandleGet(std::string path, std::shared_ptr<HttpConnection> conn)
{
    if (_get_handlers.find(path) == _get_handlers.end())
    {
        return false;
    }
    _get_handlers[path](conn);
    return true;
}

bool LogicSystem::HandlePost(std::string path, std::shared_ptr<HttpConnection> conn)
{
    if (_post_handlers.find(path) == _post_handlers.end())
    {
        return false;
    }
    _post_handlers[path](conn);
    return true;
}