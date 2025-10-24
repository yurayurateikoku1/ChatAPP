cmake_minimum_required(VERSION 3.16)
project(GateSever LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
list(PREPEND CMAKE_PREFIX_PATH 
"D:/mysql-connector-c++-8.3.0-winx64"
"E:/WORKSPACE/QtWorkSpace/ChatApp/conan_cmake_deps")

# Use Conan-provided Boost and jsoncpp config packages
find_package(Boost CONFIG REQUIRED)
find_package(jsoncpp CONFIG REQUIRED)
find_package(grpc CONFIG REQUIRED)
find_package(hiredis CONFIG REQUIRED)
find_package(mysql-concpp CONFIG REQUIRED)
# 源文件列表，确保 IntelliSense 能获取该目标的包含路径
add_executable(${PROJECT_NAME}
    main.cpp
    singleton.h
    common.h config.ini
    configmgr.h configmgr.cpp
    httpconnection.h httpconnection.cpp
    logicsystem.h logicsystem.cpp
    cserver.h cserver.cpp
    message.grpc.pb.h message.grpc.pb.cc
    message.pb.h message.pb.cc
    varifygrpcclient.h varifygrpcclient.cpp
    asioioservicepool.h asioioservicepool.cpp
    redismgr.h redismgr.cpp
    mysqldao.h mysqldao.cpp
    mysqlmgr.h mysqlmgr.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE 
Boost::headers Boost::boost Boost::filesystem Boost::system 
JsonCpp::JsonCpp  
gRPC::grpc++ gRPC::grpc 
hiredis::hiredis 
mysql::concpp
D:/mysql-connector-c++-8.3.0-winx64/lib64/vs14/mysqlcppconn.lib
)

configure_file(config.ini ${CMAKE_CURRENT_BINARY_DIR}/config.ini COPYONLY)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND}
            -DSRC_DLLS="$<TARGET_RUNTIME_DLLS:${PROJECT_NAME}>"
            -DDEST_DIR="$<TARGET_FILE_DIR:${PROJECT_NAME}>"
            -P "${CMAKE_CURRENT_SOURCE_DIR}/CopyRuntimeDependencies.cmake"
    COMMAND_EXPAND_LISTS
)

