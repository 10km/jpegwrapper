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
include_directories(
	${INCLUDE_CIMG} 
	${TURBOJPEG_INCLUDE_DIR}
	${OPENJPEG_INCLUDE_DIRS}
	${COMMONS_SOURCE_FOLDER} 
	)

set(SOURCE_FILES
		Image.cpp
	)

add_library( jpegwrapper-static  jpeg_mem.cpp j2k_mem.cpp)
add_executable( testCImg EXCLUDE_FROM_ALL testCImg.cpp )

if(NOT OPENJPEG_BUILD_SHARED_LIBS)
	##指定使用openjpeg静态库
#	add_definitions(-DOPJ_STATIC)
	target_compile_definitions (jpegwrapper-static
		PUBLIC OPJ_STATIC
		)
endif()
target_link_libraries(jpegwrapper-static ${TURBOJPEG_LIBRARY} openjp2 )
target_link_libraries(testCImg jpegwrapper-static -lpthread -lX11)

install(TARGETS jpegwrapper-static
			RUNTIME DESTINATION bin
		  LIBRARY DESTINATION lib
      ARCHIVE DESTINATION lib/static)
file(GLOB_RECURSE wrapper_headers ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
install(FILES ${wrapper_headers} DESTINATION include)
