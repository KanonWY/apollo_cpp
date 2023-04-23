# need lib

find_package(OpenSSL REQUIRED)
find_package(Protobuf REQUIRED)
find_package(srpc REQUIRED CONFIG
        HINTS "/home/kanon/Project/srpc")
if (srpc_FOUND)
    message(STATUS "---> srpc Found")
endif ()

find_package(Workflow REQUIRED CONFIG HINTS "/home/kanon/Project/srpc/workflow")
if (Workflow_FOUND)
    message(STATUS "---> WorkFlow FOUND")
endif ()

include_directories(
        ${OPENSSL_INCLUDE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
        ${Protobuf_INCLUDE_DIR}
        ${WORKFLOW_INCLUDE_DIR}
        ${SRPC_INCLUDE_DIR}
)

get_filename_component(Protobuf_LIB_DIR ${Protobuf_LIBRARY} DIRECTORY)
link_directories(${SRPC_LIB_DIR} ${WORKFLOW_LIB_DIR} ${Protobuf_LIB_DIR})
set(SRPC_GEN_PROGRAM ${SRPC_BIN_DIR}/srpc_generator)

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS message.proto)
add_custom_target(
        TURORIAL_GEN ALL
        COMMAND ${SRPC_GEN_PROGRAM} ${CMAKE_CURRENT_SOURCE_DIR}/message.proto ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "srpc generator..."
)


set(SRPC_LIB
        workflow
        pthread
        OpenSSL::SSL
        OpenSSL::Crypto
        protobuf
        z
        srpc)
add_library(apollo_param apollo_param_base.cpp srpc_backserver.cpp ${PROTO_SRCS} ${PROTO_HDRS})
target_link_libraries(apollo_param ${SRPC_LIB} apollo_cpp)
add_dependencies(apollo_param TURORIAL_GEN)


add_executable(server_test test/server_test.cc)
target_link_libraries(server_test apollo_param)

add_executable(client_test test/client_test.cc)
target_link_libraries(client_test apollo_param)
