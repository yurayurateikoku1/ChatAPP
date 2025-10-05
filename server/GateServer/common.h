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
#include <map>
#include <string>
enum ErrorCodes
{
    Success = 0,
    Error_Json = 1001,
    RPCFailed = 1002,
};

class ConfigMgr;
extern ConfigMgr gCfgMgr;