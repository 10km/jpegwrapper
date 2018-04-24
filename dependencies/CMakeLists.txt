#cmake file for dependencies 
#author:guyadong
#created:2018/04/23
cmake_minimum_required( VERSION 2.6 )
# 依赖代码位置
set(DEPENDENT_SOURCE_DIR "${PROJECT_BINARY_DIR}/dependent_sources" CACHE STRINGS "dependent source folder" FORCE)
#定义公共代码位置   
set( COMMONS_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/common_source_cpp)
file(GLOB _common_source_files ${COMMONS_SOURCE_DIR}/*)
if( _common_source_files )
	file(COPY ${_common_source_files} 
		DESTINATION ${DEPENDENT_SOURCE_DIR} 
		FILES_MATCHING
		PATTERN "*.h"
		PATTERN "*.cpp"
		)
	if(NOT MSVC)
		file(REMOVE ${DEPENDENT_SOURCE_DIR}/dirent.h)
		message(STATUS "EXCLUDE dirent.h")
	endif()
	message( STATUS "copy files of ${COMMONS_SOURCE_DIR} to ${DEPENDENT_SOURCE_DIR}" )
	unset(_common_source_files)
else()
	message( FATAL_ERROR "Empty common source folder: ${COMMONS_SOURCE_DIR},the submodule must be init and pull" )	
endif()

## WITH_STATIC_CRT 定义参见 ../cmake/compiler_flag_overrides.cmake
##############根据编译条件定义libjpeg-turbo位置##############
define_project_folder_name(libjpeg-turbo "${WITH_STATIC_CRT}")
if( IS_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/${libjpeg-turbo_folder}" )
	unset(LIBJPEG_TURBO_DIR CACHE)
	set( LIBJPEG_TURBO_DIR "${CMAKE_CURRENT_LIST_DIR}/${libjpeg-turbo_folder}" CACHE INTERNAL "libjpeg-turbo sdk folder")
	message(STATUS "LIBJPEG_TURBO_DIR=${LIBJPEG_TURBO_DIR}" )
	list(APPEND CMAKE_PREFIX_PATH ${LIBJPEG_TURBO_DIR})
elseif(MSVC)
	message(FATAL_ERROR "not found fitted libjpeg-turbo sdk directory:${libjpeg-turbo_folder},please see./README.md") 
endif()

##############根据编译条件定义openjpeg位置##############
define_project_folder_name(openjpeg "${WITH_STATIC_CRT}")
if( IS_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/${openjpeg_folder}" )
	unset(OPENJPEG_DIR CACHE)
	set( OPENJPEG_DIR "${CMAKE_CURRENT_LIST_DIR}/${openjpeg_folder}" CACHE INTERNAL "openjpeg sdk folder")
	message(STATUS "OPENJPEG_DIR=${OPENJPEG_DIR}")
	list(APPEND CMAKE_PREFIX_PATH ${OPENJPEG_DIR})
elseif(MSVC)
	message(FATAL_ERROR "not found fitted openjpeg sdk directory:${openjpeg_folder},please see./README.md") 
endif()
# 将值传递到上一级
set(CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH}" PARENT_SCOPE)
message(STATUS "CMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}" )

foreach( subdir cimg )
	if( IS_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/${subdir}" )
		MESSAGE( STATUS "Found sub project ${subdir}, adding it" )
		add_subdirectory( ${subdir} )
	endif()
endforeach()
