cmake_minimum_required(VERSION 3.0.0)
project(odbcwrap VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED on)
include(CTest)
enable_testing()


link_libraries(odbc)
# add_executable(odbcwrap main.cpp)
add_executable(odbcwrap2 main2.cpp)
# add_executable(odbcwrap3 main3.cpp)
# add_executable(odbcwrap4 main4.cpp)
# add_executable(odbcwrap5 main5.cpp)
# add_executable(odbcwrap6 main6.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)


