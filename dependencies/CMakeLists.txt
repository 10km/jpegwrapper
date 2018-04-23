#cmake file for dependencies 
#author:guyadong
#created:2018/04/23
cmake_minimum_required( VERSION 2.6 )
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
