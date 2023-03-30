find_package(PkgConfig REQUIRED)
find_package(Threads REQUIRED)
pkg_search_module(
        ASIO
        REQUIRED
        asio
        IMPORTED_TARGET
)

if(TARGET PkgConfig::ASIO)
    message(STATUS "----> find asio")
    message(STATUS "---->${ASIO_INCLUDE_DIRS} ")
    message(STATUS "---> ${ASIO_LIBRARY_DIRS}" )
    message(STATUS "---> ${ASIO_LIBRARIES}")
endif()

add_subdirectory(src)
add_subdirectory(example)