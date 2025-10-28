#pragma once
#include <vector>
#include <thread>
#include <memory>
#include <boost/asio.hpp>
#include "Singleton.h"

class AsioIOServicePool : public Singleton<AsioIOServicePool>

{

    friend Singleton<AsioIOServicePool>;

public:
    using IOService = boost::asio::io_context;

    using WorkGuard = boost::asio::executor_work_guard<IOService::executor_type>;

    using WorkGuardPtr = std::unique_ptr<WorkGuard>;

    ~AsioIOServicePool();

    AsioIOServicePool(const AsioIOServicePool &) = delete;

    AsioIOServicePool &operator=(const AsioIOServicePool &) = delete;

    // 使用 round-robin 的方式返回一个 io_service

    IOService &GetIOService();

    void Stop();

private:
    explicit AsioIOServicePool(std::size_t size = 2); // 默认两个线程
    std::vector<IOService> _ioServices;
    std::vector<WorkGuardPtr> _workGuards;
    std::vector<std::thread> _threads;
    std::size_t _nextIOService = 0;
};