cmake_minimum_required(VERSION 3.16)
project(GateSever LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
list(PREPEND CMAKE_PREFIX_PATH "D:/CPPLIBS")

# Use Conan-provided Boost and jsoncpp config packages
find_package(Boost CONFIG REQUIRED)
find_package(jsoncpp CONFIG REQUIRED)
find_package(grpc CONFIG REQUIRED)
# 源文件列表，确保 IntelliSense 能获取该目标的包含路径
add_executable(${PROJECT_NAME}
    main.cpp
    singleton.h
    common.h
    httpconnection.h httpconnection.cpp
    logicsystem.h logicsystem.cpp
    cserver.h cserver.cpp
    message.grpc.pb.h message.grpc.pb.cc
    message.pb.h message.pb.cc
    verifaygrpcclient.h verifaygrpcclient.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE Boost::headers Boost::boost JsonCpp::JsonCpp  gRPC::grpc++ gRPC::grpc)