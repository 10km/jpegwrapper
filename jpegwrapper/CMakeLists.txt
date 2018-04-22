#author:guyadong
#created:2016/01/16
############################################################################
cmake_minimum_required( VERSION 2.6 )
find_package(JPEG REQUIRED)
message(STATUS "JPEG_LIBRARIES=${JPEG_LIBRARIES}")
message(STATUS "JPEG_LIBRARY=${JPEG_LIBRARY}")
message(STATUS "JPEG_INCLUDE_DIR=${JPEG_INCLUDE_DIR}")
get_filename_component(LIBJPEG_TURBO_LIBRARY_DIR "${JPEG_LIBRARY}" PATH)
if(MSVC)
	set(_stati_library_name ${CMAKE_STATIC_LIBRARY_PREFIX}turbojpeg-static${CMAKE_STATIC_LIBRARY_SUFFIX})
else()
	set(_stati_library_name ${CMAKE_STATIC_LIBRARY_PREFIX}turbojpeg${CMAKE_STATIC_LIBRARY_SUFFIX})
endif()
find_library(LIBJPEG_TURBO_LIBRARY_STATIC 
	name ${_stati_library_name}
	PATHS ${LIBJPEG_TURBO_LIBRARY_DIR} 
	NO_DEFAULT_PATH)
if(LIBJPEG_TURBO_LIBRARY_STATIC)
	message(STATUS "LIBJPEG_TURBO_LIBRARY_STATIC=${LIBJPEG_TURBO_LIBRARY_STATIC}")
	set (JPEG_LIBRARY ${LIBJPEG_TURBO_LIBRARY_STATIC})
else()
	message(STATUS "NOT FOUND jpeg-turbo static library")
endif()
find_package(OpenJPEG CONFIG REQUIRED)
message(STATUS "OPENJPEG_LIBRARIES=${OPENJPEG_LIBRARIES}")
message(STATUS "OPENJPEG_INCLUDE_DIRS=${OPENJPEG_INCLUDE_DIRS}")

# 加入cimg openjpeg libjpeg 及公共代码支持
include_directories(
	${INCLUDE_CIMG} 
	${JPEG_INCLUDE_DIR}
	${OPENJPEG_INCLUDE_DIRS}
	${COMMONS_SOURCE_FOLDER} 
	)

set(SOURCE_FILES
		Image.cpp
	)
#add_executable( cimgagent ${SOURCE_FILES})
add_library( jpegwrapper-static  jpeg_mem.cpp j2k_mem.cpp)
add_executable( testCImg EXCLUDE_FROM_ALL testCImg.cpp )

if(OPENJPEG_BUILD_SHARED_LIBS)
	message(STATUS "Link OpenJPEG shared library: ${OPENJPEG_LIBRARIES}")
else()
	##指定使用openjpeg静态库
	add_definitions(-DOPJ_STATIC)
endif()
target_link_libraries(jpegwrapper-static ${JPEG_LIBRARY} openjp2 )
target_link_libraries(testCImg jpegwrapper-static)

install(TARGETS jpegwrapper-static
			RUNTIME DESTINATION bin
		  LIBRARY DESTINATION lib
      ARCHIVE DESTINATION lib/static)
file(GLOB_RECURSE wrapper_headers ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
install(FILES ${wrapper_headers} DESTINATION include)
