cmake_minimum_required(VERSION 3.20)
project(GateServer)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)

include (cmake/utils.cmake)
include_directories(${PROJECT_SOURCE_DIR}/include)

# 查找依赖库
find_package(Boost REQUIRED) 
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
endif()
find_package(jsoncpp REQUIRED) 
if(jsoncpp_FOUND)
    include_directories(${jsoncpp_INCLUDE_DIRS})
endif()

# 查找MySQL Connector/C++
find_path(MYSQL_CONNECTOR_INCLUDE_DIR
    NAMES mysql_driver.h
    PATHS /usr/include
    REQUIRED
)

find_library(MYSQL_CPPCONN_LIB
    NAMES mysqlcppconn
    PATHS /usr/lib/x86_64-linux-gnu
    REQUIRED
)

if(MYSQL_CONNECTOR_INCLUDE_DIR AND MYSQL_CPPCONN_LIB)
    message(STATUS "Found MySQL Connector/C++ (JDBC version)")
    message(STATUS "  Include: ${MYSQL_CONNECTOR_INCLUDE_DIR}")
    message(STATUS "  Library: ${MYSQL_CPPCONN_LIB}")
    include_directories(${MYSQL_CONNECTOR_INCLUDE_DIR})
endif()

# 添加 gRPC 和 Protobuf 依赖
find_package(Threads REQUIRED)

set(protobuf_MODULE_COMPATIBLE TRUE)
find_package(Protobuf CONFIG REQUIRED)
message(STATUS "Using protobuf ${Protobuf_VERSION}")

find_package(gRPC CONFIG REQUIRED)
message(STATUS "Using gRPC ${gRPC_VERSION}")

# protobuf 31.1.0
# gRPC 1.76.0
# 设置 gRPC 相关变量
set(_PROTOBUF_LIBPROTOBUF protobuf::libprotobuf)
set(_REFLECTION gRPC::grpc++_reflection)
set(_GRPC_GRPCPP gRPC::grpc++)

# 查找 protoc 和 grpc_cpp_plugin
find_program(PROTOC protoc)
find_program(GRPC_CPP_PLUGIN grpc_cpp_plugin)

# 添加源文件
set(LIB_SRC
    src/CServer.cpp
    src/HttpConnection.cpp
    src/LogicSystem.cpp
    src/ConfigMgr.cpp
    src/VerifyGrpcClient.cpp
    src/AsioIOServicePool.cpp
    src/RedisMgr.cpp
    src/MysqlDao.cpp
    src/MysqlMgr.cpp
    src/StatusGrpcClient.cpp
    )

add_library(GateServer SHARED ${LIB_SRC})

set(LIBS
    GateServer
    jsoncpp
    hiredis
    ${_REFLECTION}
    ${_GRPC_GRPCPP}
    ${_PROTOBUF_LIBPROTOBUF}
    Threads::Threads
    ${MYSQL_CPPCONN_LIB} 
)

# 编译 proto 文件
COMPILE_PROTO(${PROJECT_SOURCE_DIR}/proto/message.proto)

# 添加可执行文件
GateServer_add_executable(CServer "src/main.cpp" GateServer "${LIBS}")

# 设置输出目录
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)