#pragma once
#include <boost/beast/http.hpp>
#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <json/json.h>
#include <json/value.h>
#include <json/reader.h>
#include <memory>
#include <iostream>
#include <queue>
#include <map>
#include <string>
#include <hiredis/hiredis.h>
enum ErrorCodes
{
    Success = 0,
    Error_Json = 1001,
    RPCFailed = 1002,
    VarifyExpired = 1003,
    VarifyCodeErr = 1004,
    UserExist = 1005,
    PasswdErr = 1006,
    EmailNotMatch = 1007,
    PasswdUpFailed = 1008,
    PasswdInValid = 1009
};

#define CODEPREFIX "code_"

class Defer
{
public:
    Defer(std::function<void()> func) : _func(func) {}
    ~Defer()
    {
        _func();
    }

private:
    std::function<void()> _func;
};