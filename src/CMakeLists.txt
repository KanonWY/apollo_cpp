find_package(cpprestsdk REQUIRED)

add_executable(testHttp testHttp.cpp)
target_link_libraries(testHttp PRIVATE cpprestsdk::cpprest)

add_library(apollo_cpp SHARED apollo_client.cpp apollo_base.cpp )
target_link_libraries(apollo_cpp PUBLIC cpprestsdk::cpprest)


add_executable(test test.cpp)
target_link_libraries(test apollo_cpp)