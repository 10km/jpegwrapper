#cmake file for project opencl
#author:guyadong
#created:2016/01/09
cmake_minimum_required( VERSION 2.6 )
project(jpegwrapper)

#设置处理器
if(TARGET_PROCESSOR)
	set (CMAKE_SYSTEM_PROCESSOR ${TARGET_PROCESSOR})
endif()
if(MSVC)     
	#关闭C4819警告
	add_definitions("/wd4819")		
	message(STATUS "optional:/wd4819")
	#关闭CRT_SECURE警告
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
	message(STATUS "optional:-D_CRT_SECURE_NO_WARNINGS")	
    # Use the static C library for all build types
    foreach(var 
    	CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE
      	CMAKE_C_FLAGS_MINSIZEREL CMAKE_C_FLAGS_RELWITHDEBINFO
      	CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
      	CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO
      )
      if(${var} MATCHES "/MD")
        string(REGEX REPLACE "/MD" "/MT" ${var} "${${var}}")
      endif()
    endforeach()	
endif(MSVC)
#定义公共代码位置   
set( COMMONS_SOURCE_FOLDER ${CMAKE_SOURCE_DIR}/common_source_cpp CACHE STRING "common source folder" FORCE)
if( NOT IS_DIRECTORY "${COMMONS_SOURCE_FOLDER}" )
	MESSAGE( FATAL_ERROR "Found common source folder: ${COMMONS_SOURCE_FOLDER}, please locate it" )	
endif()
MESSAGE( STATUS "COMMONS_SOURCE_FOLDER= ${COMMONS_SOURCE_FOLDER}" )
##############根据编译条件定义libjpeg-turbo位置##############
unset(LIBJPEG_TURBO_DIR CACHE)
if ( MSVC AND WIN32 AND CMAKE_SYSTEM_PROCESSOR MATCHES "AMD64|x86_64")
	set ( libjpeg_turbo_flolder ${CMAKE_CURRENT_LIST_DIR}/libjpeg-turbo-windows-vc-x86_64 )
elseif(MSVC AND WIN32 AND CMAKE_SYSTEM_PROCESSOR MATCHES "x86")
	set ( libjpeg_turbo_flolder ${CMAKE_CURRENT_LIST_DIR}/libjpeg-turbo-windows-vc-x86 )
elseif(CMAKE_COMPILER_IS_GNUCXX AND WIN32 AND CMAKE_SYSTEM_PROCESSOR MATCHES "AMD64|x86_64")
	set ( libjpeg_turbo_flolder ${CMAKE_CURRENT_LIST_DIR}/libjpeg-turbo-windows-gcc-x86_64 )
elseif(CMAKE_COMPILER_IS_GNUCXX AND WIN32 AND CMAKE_SYSTEM_PROCESSOR MATCHES "x86")
	set ( libjpeg_turbo_flolder ${CMAKE_CURRENT_LIST_DIR}/libjpeg-turbo-windows-gcc-x86 )
elseif(CMAKE_COMPILER_IS_GNUCXX AND CMAKE_SYSTEM_NAME MATCHES "Linux" AND CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
	set ( libjpeg_turbo_flolder ${CMAKE_CURRENT_LIST_DIR}/libjpeg-turbo-linux-x86_64 )
elseif(CMAKE_COMPILER_IS_GNUCXX AND CMAKE_SYSTEM_NAME MATCHES "Linux" AND CMAKE_SYSTEM_PROCESSOR MATCHES "x86")
	set ( libjpeg_turbo_flolder ${CMAKE_CURRENT_LIST_DIR}/libjpeg-turbo-linux-x86 )
else()
	message(FATAL_ERROR "not yet supported host:${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR} compiled by ${CMAKE_CXX_COMPILER},please see./README.txt") 
endif()

if( NOT IS_DIRECTORY "${libjpeg_turbo_flolder}" )
	message(FATAL_ERROR "not found fitted libjpeg-turbo sdk directory:${libjpeg_turbo_flolder},please see./README.txt") 
endif()

set( LIBJPEG_TURBO_DIR "${libjpeg_turbo_flolder}" CACHE INTERNAL "libjpeg-turbo sdk folder")
message(STATUS "LIBJPEG_TURBO_DIR=${LIBJPEG_TURBO_DIR}" )

##############根据编译条件定义openjpeg位置##############
unset(OPENJPEG_DIR CACHE)
if ( MSVC AND WIN32 AND CMAKE_SYSTEM_PROCESSOR MATCHES "AMD64|x86_64")
	set ( openjpeg_flolder ${CMAKE_CURRENT_LIST_DIR}/openjpeg-windows-vc-x86_64 )
elseif( MSVC AND WIN32 AND CMAKE_SYSTEM_PROCESSOR MATCHES "x86")
	set ( openjpeg_flolder ${CMAKE_CURRENT_LIST_DIR}/openjpeg-windows-vc-x86 )
elseif( CMAKE_COMPILER_IS_GNUCXX AND WIN32 AND CMAKE_SYSTEM_PROCESSOR MATCHES "AMD64|x86_64")
	set ( openjpeg_flolder ${CMAKE_CURRENT_LIST_DIR}/openjpeg-windows-gcc-x86_64 )
elseif( CMAKE_COMPILER_IS_GNUCXX AND WIN32 AND CMAKE_SYSTEM_PROCESSOR MATCHES "x86")
	set ( openjpeg_flolder ${CMAKE_CURRENT_LIST_DIR}/openjpeg-windows-gcc-x86 )	
elseif(CMAKE_COMPILER_IS_GNUCXX AND CMAKE_SYSTEM_NAME MATCHES "Linux" AND CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
	set ( openjpeg_flolder ${CMAKE_CURRENT_LIST_DIR}/openjpeg-linux-gcc-x86_64 )
elseif(CMAKE_COMPILER_IS_GNUCXX AND CMAKE_SYSTEM_NAME MATCHES "Linux" AND CMAKE_SYSTEM_PROCESSOR MATCHES "x86")
	set ( openjpeg_flolder ${CMAKE_CURRENT_LIST_DIR}/openjpeg-linux-gcc-x86 )
else()
	message(FATAL_ERROR "not yet supported host:${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR} compiled by ${CMAKE_CXX_COMPILER},please see./README.txt") 
endif()

if( NOT IS_DIRECTORY "${openjpeg_flolder}" )
	message(FATAL_ERROR "not found fitted openjpeg sdk directory:${openjpeg_flolder},please see./README.txt") 
endif()

set( OPENJPEG_DIR "${openjpeg_flolder}" CACHE INTERNAL "openjpeg sdk folder")
message(STATUS "OPENJPEG_DIR=${OPENJPEG_DIR}" )

## 对给定的library_name,在lib_dir中查找import library和静态库，
## 分别定义为${library_name}(大写)_LIBRARY_STATIC和{library_name}(大写)_LIBRARY_IMPORT全局变量
function(defineLibraryVariable library_name lib_dir)
	##计算${library_name}的静态库名
	string(TOUPPER ${library_name} var_prefix)
	if( MSVC)
		set ( static_library_name ${library_name}-static${CMAKE_STATIC_LIBRARY_SUFFIX})
	elseif(CMAKE_COMPILER_IS_GNUCXX)	
		set ( static_library_name lib${library_name}${CMAKE_STATIC_LIBRARY_SUFFIX})
	endif()
	##查找${library_name}的静态库
	unset(${var_prefix}_LIBRARY_STATIC CACHE)
	find_library( ${var_prefix}_LIBRARY_STATIC
		NAMES ${static_library_name}
		PATHS "${lib_dir}/lib"
		DOC "${library_name} static library"
		NO_DEFAULT_PATH
	)
	message(STATUS "${var_prefix}_LIBRARY_STATIC=${${var_prefix}_LIBRARY_STATIC}" )
	##查找${library_name}的动态导入库(import library)
	unset(${var_prefix}_LIBRARY_IMPORT CACHE)
	find_library( ${var_prefix}_LIBRARY_IMPORT
		NAMES ${library_name}
		PATHS "${lib_dir}/lib"
		DOC "${var_prefix} import library"
		NO_DEFAULT_PATH
	)
	message(STATUS "${var_prefix}_LIBRARY_IMPORT=${${var_prefix}_LIBRARY_IMPORT}" )
endfunction()
##定义jpeg静态库和动态导入库
defineLibraryVariable(jpeg ${LIBJPEG_TURBO_DIR})
##定义turbojpeg静态库和动态导入库
defineLibraryVariable(turbojpeg ${LIBJPEG_TURBO_DIR})
##定义openjpeg静态库和动态导入库
defineLibraryVariable(openjp2 ${OPENJPEG_DIR})


##############定义CImg位置##############
set(INCLUDE_CIMG "${CMAKE_CURRENT_LIST_DIR}/CImg" CACHE INTERNAL "CImg include folder" )
message(STATUS "INCLUDE_CIMG=${INCLUDE_CIMG}" )


set( SUBDIRECTORIES jpegwrapper)
foreach( subdir ${SUBDIRECTORIES} )
	if( IS_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/${subdir}" )
		MESSAGE( STATUS "Found sub project ${subdir}, adding it" )
		add_subdirectory( ${subdir} )
	endif()
endforeach()

install(FILES ${JPEG_LIBRARY_STATIC} ${OPENJP2_LIBRARY_STATIC} DESTINATION lib)
install(FILES ${INCLUDE_CIMG}/CImg.h DESTINATION include)
install(FILES ${COMMONS_SOURCE_FOLDER}/raii.h DESTINATION include)
install(DIRECTORY ${LIBJPEG_TURBO_DIR}/include DESTINATION .)
install(DIRECTORY ${OPENJPEG_DIR}/include DESTINATION .)
