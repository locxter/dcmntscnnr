cmake_minimum_required(VERSION 3.0.0)
project(dcmntscnnr VERSION 1.0)

include(CTest)
enable_testing()

add_executable(dcmntscnnr main.cpp lib/image-previewer.hpp lib/image-previewer.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

find_package(OpenCV)
find_package(PkgConfig)
pkg_check_modules(GTKMM gtkmm-3.0)
include_directories(${OpenCV_INCLUDE_DIRS} ${GTKMM_INCLUDE_DIRS})
target_link_libraries(dcmntscnnr ${OpenCV_LIBS} ${GTKMM_LIBRARIES})
