#pragma once
#include <boost/beast/http.hpp>
#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include <json/json.h>
#include <json/value.h>
#include <json/reader.h>
#include <memory>
#include <iostream>

enum ErrorCode
{
    Success = 0,
    Error_Json = 1001,
    PRCFailed = 1002,
};