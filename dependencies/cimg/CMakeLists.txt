#cmake file for CImg
#author:guyadong
#created:2018/04/23
cmake_minimum_required( VERSION 2.6 )
##############定义CImg.h位置##############
set(CIMG_H "${CMAKE_CURRENT_LIST_DIR}/CImg/CImg.h")
# 将 CImg.h复制到依赖代码文件夹
file(COPY ${CIMG_H} DESTINATION ${DEPENDENT_SOURCE_DIR})
message( STATUS "copy ${CIMG_H} to ${DEPENDENT_SOURCE_DIR}" )

add_library(cimg INTERFACE)
target_include_directories(cimg INTERFACE
  $<BUILD_INTERFACE:${DEPENDENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:include>
)
if(CMAKE_SYSTEM_NAME MATCHES "Linux")
	target_link_libraries(cimg INTERFACE -lX11)
endif()
if(CMAKE_COMPILER_IS_GNUCXX)
	target_link_libraries(cimg INTERFACE -lpthread)
endif()
