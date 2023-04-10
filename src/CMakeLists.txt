# get depend lib
find_package(cpprestsdk REQUIRED)
find_package(spdlog REQUIRED)
find_package(yaml-cpp REQUIRED)

# get cpp file
file(GLOB_RECURSE SRC_FILES "*.cpp")

if (GEN_SHARED_LIB)
    add_library(apollo_cpp SHARED ${SRC_FILES})
    target_link_libraries(apollo_cpp PUBLIC cpprestsdk::cpprest spdlog yaml-cpp tinyxml2)
    target_include_directories(apollo_cpp PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
endif ()


if (GEN_STATIC_LIB)
    add_library(apollo_cpp_static STATIC ${SRC_FILES})
    target_link_libraries(apollo_cpp_static PUBLIC cpprestsdk::cpprest spdlog)
    target_include_directories(apollo_cpp_static PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
endif ()