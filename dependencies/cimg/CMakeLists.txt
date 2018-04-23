#cmake file for CImg
#author:guyadong
#created:2018/05/23
cmake_minimum_required( VERSION 2.6 )
##############定义CImg位置##############
unset(INCLUDE_CIMG CACHE)
set(INCLUDE_CIMG "${CMAKE_CURRENT_LIST_DIR}/CImg" CACHE INTERNAL "CImg include folder" )
message(STATUS "INCLUDE_CIMG=${INCLUDE_CIMG}" )
add_library(cimg INTERFACE)
target_include_directories(cimg INTERFACE
  $<BUILD_INTERFACE:${INCLUDE_CIMG}>
  $<INSTALL_INTERFACE:include/cimg>
)
if(CMAKE_SYSTEM_NAME MATCHES "Linux")
	target_link_libraries(cimg INTERFACE -lX11)
endif()
if(CMAKE_COMPILER_IS_GNUCXX)
	target_link_libraries(cimg INTERFACE -lpthread)
endif()
