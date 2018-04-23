#author:guyadong
#created:2016/01/16
############################################################################
cmake_minimum_required( VERSION 2.6 )
find_package(TurboJPEG REQUIRED)
message(STATUS "TURBOJPEG_LIBRARY=${TURBOJPEG_LIBRARY}")
message(STATUS "TURBOJPEG_INCLUDE_DIR=${TURBOJPEG_INCLUDE_DIR}")
if(TURBOJPEG_LIBRARY_STATIC)
	set (TURBOJPEG_LIBRARY ${TURBOJPEG_LIBRARY_STATIC})
endif()
find_package(OpenJPEG CONFIG REQUIRED)
message(STATUS "OPENJPEG_LIBRARIES=${OPENJPEG_LIBRARIES}")
message(STATUS "OPENJPEG_BUILD_SHARED_LIBS=${OPENJPEG_BUILD_SHARED_LIBS}")
message(STATUS "OPENJPEG_INCLUDE_DIRS=${OPENJPEG_INCLUDE_DIRS}")

# 加入cimg openjpeg libjpeg 及公共代码支持
#include_directories(
#	${INCLUDE_CIMG} 
#	${TURBOJPEG_INCLUDE_DIR}
#	${OPENJPEG_INCLUDE_DIRS}
#	${COMMONS_SOURCE_FOLDER} 
#	)

##### define static library ######
set(SOURCE_FILES
	jpeg_mem.cpp 
	j2k_mem.cpp
	)
set(_target_static jpegwrapper-static)
add_library( ${_target_static}  ${SOURCE_FILES})
if(NOT OPENJPEG_BUILD_SHARED_LIBS)
	##指定使用openjpeg静态库
#	add_definitions(-DOPJ_STATIC)
	target_compile_definitions (${_target_static}
		PUBLIC OPJ_STATIC
		)
endif()
target_include_directories (${_target_static} 
	PRIVATE ${COMMONS_SOURCE_FOLDER}
	PUBLIC ${TURBOJPEG_INCLUDE_DIR}
	PUBLIC ${OPENJPEG_INCLUDE_DIRS}
	)

target_link_libraries(${_target_static} ${TURBOJPEG_LIBRARY} openjp2 )
##### define testCImg #######
add_executable( testCImg EXCLUDE_FROM_ALL testCImg.cpp )
target_include_directories (testCImg BEFORE 
	PRIVATE ${COMMONS_SOURCE_FOLDER} 
	PRIVATE ${INCLUDE_CIMG}
	)
target_link_libraries(testCImg ${_target_static} -lpthread -lX11)

install(TARGETS ${_target_static}
			RUNTIME DESTINATION bin
		  LIBRARY DESTINATION lib
      ARCHIVE DESTINATION lib/static)
file(GLOB_RECURSE wrapper_headers ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
install(FILES ${wrapper_headers} DESTINATION include)

unset(_target_static)
