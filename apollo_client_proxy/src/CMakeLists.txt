add_subdirectory(protoc)


file(GLOB_RECURSE CPP_FILES "*.cpp")

message(STATUS "----> CPP_FILES ${CPP_FILES}")

add_library(Easy_tcp ${CPP_FILES})
target_link_libraries(Easy_tcp gprotoc apollo_cpp)
target_include_directories(Easy_tcp PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})








# # 1. gen cpp file by proto file, then package into a obj.
# find_package(Protobuf REQUIRED)
# include_directories(${PROTOBUF_INCLUDE_DIRS})

# set(PROTO_FILES "protobuf/msg.proto")
# protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_FILES})

# #add_library(PB_OBJS OBJECT ${PROTO_SRCS})



# # 2. gen share lib
# add_library(Easy_tcp_helper SHARED ETcpServer.cpp basemessage.cpp ${PROTO_SRCS})
# target_link_libraries(Easy_tcp_helper ${PROTOBUF_LIBRARY})


# file(GLOB_RECURSE TCP_SOURCE_FILE "*.cpp")
# file(GLOB_RECURSE TCP_SOURCE_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.cpp")

# add_library(Easy_tcp SHARED ${TCP_SOURCE_FILE})



