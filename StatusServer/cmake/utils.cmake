# 添加可执行文件函数
function(GateServer_add_executable targetname srcs depends libs)
    add_executable(${targetname} ${srcs})
    add_dependencies(${targetname} ${depends})
    target_include_directories(${targetname} PRIVATE ${PROJECT_SOURCE_DIR}/include)
    target_link_libraries(${targetname} ${libs})
endfunction()


# 添加 proto 文件编译函数
function(COMPILE_PROTO proto_file)
    get_filename_component(proto_path ${proto_file} DIRECTORY)
    get_filename_component(proto_name ${proto_file} NAME_WE)
    
    set(proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/${proto_name}.pb.cc")
    set(proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/${proto_name}.pb.h")
    set(grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/${proto_name}.grpc.pb.cc")
    set(grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/${proto_name}.grpc.pb.h")
    
    # 生成 protobuf 文件
    add_custom_command(
        OUTPUT "${proto_srcs}" "${proto_hdrs}"
        COMMAND ${PROTOC}
        ARGS --cpp_out ${CMAKE_CURRENT_BINARY_DIR}
             -I ${PROJECT_SOURCE_DIR}/proto
             ${proto_file}
        DEPENDS ${proto_file}
    )
    
    # 生成 gRPC 文件
    add_custom_command(
        OUTPUT "${grpc_srcs}" "${grpc_hdrs}"
        COMMAND ${PROTOC}
        ARGS --grpc_out ${CMAKE_CURRENT_BINARY_DIR}
             --plugin=protoc-gen-grpc=${GRPC_CPP_PLUGIN}
             -I ${PROJECT_SOURCE_DIR}/proto
             ${proto_file}
        DEPENDS ${proto_file}
    )
    
    # 将生成的文件添加到库中
    target_sources(StatusServer PRIVATE ${proto_srcs} ${grpc_srcs})
    
    # 添加生成目录到头文件路径
    target_include_directories(StatusServer PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

    # 链接 Protobuf 库，这会自动添加正确的包含路径
    target_link_libraries(StatusServer PRIVATE ${_PROTOBUF_LIBPROTOBUF})
endfunction()
