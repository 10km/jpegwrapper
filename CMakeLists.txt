#cmake file for project opencl
#author:guyadong
#created:2016/01/09
cmake_minimum_required( VERSION 2.6 )
project(jpegwrapper)

#设置处理器
if(TARGET_PROCESSOR)
	set (CMAKE_SYSTEM_PROCESSOR ${TARGET_PROCESSOR})
endif()
# includes utils.cmake module
set (CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/Modules")
include (utils)
cxx11_support()
if(MSVC)
	#关闭C4819警告
	add_definitions("/wd4819")		
	message(STATUS "optional:/wd4819")
	#关闭CRT_SECURE警告
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
	message(STATUS "optional:-D_CRT_SECURE_NO_WARNINGS")
endif(MSVC)
#定义公共代码位置   
set( COMMONS_SOURCE_FOLDER ${CMAKE_SOURCE_DIR}/common_source_cpp CACHE STRING "common source folder" FORCE)
if( NOT IS_DIRECTORY "${COMMONS_SOURCE_FOLDER}" )
	message( FATAL_ERROR "Found common source folder: ${COMMONS_SOURCE_FOLDER}, please locate it" )	
endif()
message( STATUS "COMMONS_SOURCE_FOLDER= ${COMMONS_SOURCE_FOLDER}" )

##############根据编译条件定义libjpeg-turbo位置##############
define_project_folder_name(libjpeg-turbo "${WITH_STATIC_CRT}")
if( MSVC AND NOT IS_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/${libjpeg-turbo_folder}" )
	message(FATAL_ERROR "not found fitted libjpeg-turbo sdk directory:${libjpeg-turbo_folder},please see./README.md") 
endif()
unset(LIBJPEG_TURBO_DIR CACHE)
set( LIBJPEG_TURBO_DIR "${CMAKE_CURRENT_LIST_DIR}/${libjpeg-turbo_folder}" CACHE INTERNAL "libjpeg-turbo sdk folder")
message(STATUS "LIBJPEG_TURBO_DIR=${LIBJPEG_TURBO_DIR}" )
list(APPEND CMAKE_PREFIX_PATH ${LIBJPEG_TURBO_DIR})
##############根据编译条件定义openjpeg位置##############
define_project_folder_name(openjpeg "${WITH_STATIC_CRT}")
if( MSVC AND NOT IS_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/${openjpeg_folder}" )
	message(FATAL_ERROR "not found fitted openjpeg sdk directory:${openjpeg_folder},please see./README.md") 
endif()
unset(OPENJPEG_DIR CACHE)
set( OPENJPEG_DIR "${CMAKE_CURRENT_LIST_DIR}/${openjpeg_folder}" CACHE INTERNAL "openjpeg sdk folder")
message(STATUS "OPENJPEG_DIR=${OPENJPEG_DIR}" )
list(APPEND CMAKE_PREFIX_PATH ${OPENJPEG_DIR})
message(STATUS "CMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}" )
##############定义CImg位置##############
set(INCLUDE_CIMG "${CMAKE_CURRENT_LIST_DIR}/CImg" CACHE INTERNAL "CImg include folder" )
message(STATUS "INCLUDE_CIMG=${INCLUDE_CIMG}" )

##############设置目标文件生成位置#####################
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")
set( SUBDIRECTORIES jpegwrapper)
foreach( subdir ${SUBDIRECTORIES} )
	if( IS_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/${subdir}" )
		MESSAGE( STATUS "Found sub project ${subdir}, adding it" )
		add_subdirectory( ${subdir} )
	endif()
endforeach()

#install(FILES ${JPEG_LIBRARY_STATIC} ${OPENJP2_LIBRARY_STATIC} DESTINATION lib/static)
install(FILES ${INCLUDE_CIMG}/CImg.h DESTINATION include)
install(FILES ${COMMONS_SOURCE_FOLDER}/raii.h DESTINATION include)
#install(DIRECTORY ${LIBJPEG_TURBO_DIR}/include DESTINATION .)
#install(DIRECTORY ${OPENJPEG_DIR}/include DESTINATION .)
