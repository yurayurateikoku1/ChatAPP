#include "logicsystem.h"
#include "httpconnection.h"
#include "varifygrpcclient.h"
#include "redismgr.h"
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

    RegPost("/user_register", [](std::shared_ptr<HttpConnection> connection)
            {
    auto body_str = boost::beast::buffers_to_string(connection->_request.body().data());
    std::cout << "receive body is " << body_str << std::endl;
    connection->_response.set(boost::beast::http::field::content_type, "text/json");
    Json::Value root;
    Json::Reader reader;
    Json::Value src_root;
    bool parse_success = reader.parse(body_str, src_root);
    if (!parse_success) {
        std::cout << "Failed to parse JSON data!" << std::endl;
        root["error"] = ErrorCodes::Error_Json;
        std::string jsonstr = root.toStyledString();
        boost::beast::ostream(connection->_response.body()) << jsonstr;
        return true;
    }
    //先查找redis中email对应的验证码是否合理
    std::string  varify_code;
    bool b_get_varify = RedisMgr::GetInstance()->Get(CODEPREFIX+ src_root["email"].asString(), varify_code);
    if (!b_get_varify) {
        std::cout << " get varify code expired" << std::endl;
        root["error"] = ErrorCodes::VarifyExpired;
        std::string jsonstr = root.toStyledString();
        boost::beast::ostream(connection->_response.body()) << jsonstr;
        return true;
    }
    if (varify_code != src_root["varifycode"].asString()) {
        std::cout << " varify code error" << std::endl;
        root["error"] = ErrorCodes::VarifyCodeErr;
        std::string jsonstr = root.toStyledString();
        boost::beast::ostream(connection->_response.body()) << jsonstr;
        return true;
    }
    //访问redis查找
    // bool b_usr_exist = RedisMgr::GetInstance()->ExistsKey(src_root["user"].asString());
    // if (b_usr_exist) {
    //     std::cout << " user exist" << std::endl;
    //     root["error"] = ErrorCodes::UserExist;
    //     std::string jsonstr = root.toStyledString();
    //     boost::beast::ostream(connection->_response.body()) << jsonstr;
    //     return true;
    // }
    //查找数据库判断用户是否存在
    root["error"] = 0;
    root["email"] = src_root["email"];
    root ["user"]= src_root["user"].asString();
    root["passwd"] = src_root["passwd"].asString();
    root["confirm"] = src_root["confirm"].asString();
    root["varifycode"] = src_root["varifycode"].asString();
    std::string jsonstr = root.toStyledString();
    boost::beast::ostream(connection->_response.body()) << jsonstr;
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