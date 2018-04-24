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


##############设置目标文件生成位置#####################
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")
set( SUBDIRECTORIES dependencies jpegwrapper)
foreach( subdir ${SUBDIRECTORIES} )
	if( IS_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/${subdir}" )
		MESSAGE( STATUS "Found sub project ${subdir}, adding it" )
		add_subdirectory( ${subdir} )
	endif()
endforeach()

#install(FILES ${JPEG_LIBRARY_STATIC} ${OPENJP2_LIBRARY_STATIC} DESTINATION lib/static)
#install(DIRECTORY ${LIBJPEG_TURBO_DIR}/include DESTINATION .)
#install(DIRECTORY ${OPENJPEG_DIR}/include DESTINATION .)
