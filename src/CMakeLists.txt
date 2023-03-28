# get depend lib
find_package(cpprestsdk REQUIRED)
find_package(spdlog REQUIRED)
find_package(yaml-cpp REQUIRED)

if(GEN_SHARED_LIB)
    add_library(apollo_cpp SHARED apollo_client_mulns.cpp apollo_client_singlens.cpp apollo_base.cpp apollo_client.cpp apollo_client_mul_yaml.cpp)
    target_link_libraries(apollo_cpp PUBLIC cpprestsdk::cpprest spdlog yaml-cpp)
    target_include_directories(apollo_cpp PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
endif()


if(GEN_STATIC_LIB)
    add_library(apollo_cpp_static STATIC apollo_client_mulns.cpp apollo_client_singlens.cpp apollo_base.cpp apollo_client.cpp)
    target_link_libraries(apollo_cpp_static PUBLIC cpprestsdk::cpprest spdlog)
    target_include_directories(apollo_cpp_static PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
endif()