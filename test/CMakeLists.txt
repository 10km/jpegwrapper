#cmake file for test refer to jpegwrapper-static 
#author:guyadong
#created:2018/04/25
cmake_minimum_required( VERSION 3.0 )
project( testjpegwrapper LANGUAGES CXX)
#判断编译类型和版本是否满足编译要求
if(MSVC AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 19)
	message(STATUS "CMAKE_CXX_COMPILER: ${CMAKE_CXX_COMPILER}") 
	message(STATUS "CMAKE_CXX_COMPILER_VERSION=${CMAKE_CXX_COMPILER_VERSION}" )
elseif(CMAKE_COMPILER_IS_GNUCXX AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 5)
	message(STATUS "CMAKE_CXX_COMPILER: ${CMAKE_CXX_COMPILER}") 
	message(STATUS "CMAKE_CXX_COMPILER_VERSION=${CMAKE_CXX_COMPILER_VERSION}" )
else()
	message(FATAL_ERROR "compiler required: Visual Studio 2015 OR gcc 5.2.0" )
endif()
# 查找jpegwrapper库,需要设置 CMAKE_PREIX_PATH 指定 jpegwrapper,turbojpeg,openjpeg的安装路径
# 需要指定CMAKE_MODULE_PATH 指定 FindTurboJPEG.cmake 的位置
# 本例中的位置在项目根目录下/cmake/Modules
find_package(jpegwrapper CONFIG REQUIRED)
# include utils.cmake,location /cmake/Modules
include (utils)
cxx11_support()
# 定义所有的源文件列表  
set(_CPP_SOURCE_FILES
    testCImg.cpp 
    )

add_executable(test_jpegwrapper ${_CPP_SOURCE_FILES})
target_include_directories (test_jpegwrapper PUBLIC ${OPENJPEG_INCLUDE_DIRS})
target_link_libraries(test_jpegwrapper gdface::jpegwrapper-static $<$<PLATFORM_ID:Linux>:-lX11> $<$<CXX_COMPILER_ID:GNU>:-lpthread>)
target_compile_options(test_jpegwrapper PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/wd4996 /wd4819>)
if(NOT ${PROJECT_BINARY_DIR} STREQUAL ${PROJECT_SOURCE_DIR}) 
	# 复制测试图像到${PROJECT_BINARY_DIR}
	file(RELATIVE_PATH _current_folder ${PROJECT_SOURCE_DIR} ${CMAKE_CURRENT_LIST_DIR})
	file(COPY ${CMAKE_CURRENT_LIST_DIR}/images DESTINATION ${PROJECT_BINARY_DIR}/${_current_folder})
	message(STATUS "copy ${CMAKE_CURRENT_LIST_DIR}/images to ${PROJECT_BINARY_DIR}/${_current_folder}")
	unset(_current_folder)
endif()


# Cleanup temporary variables.
unset(_CPP_SOURCE_FILES)
