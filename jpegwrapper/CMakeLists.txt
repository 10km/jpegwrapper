#author:guyadong
#created:2016/01/16
############################################################################
cmake_minimum_required( VERSION 2.6 )
# 加入cimg openjpeg libjpeg 及公共代码支持
include_directories(
	${INCLUDE_CIMG} 
	${LIBJPEG_TURBO_DIR}/include 
	${OPENJPEG_DIR}/include 
	${COMMONS_SOURCE_FOLDER} 
	${PROJECT_SOURCE_DIR}/include
	)

set(SOURCE_FILES
		Image.cpp
	)
#add_executable( cimgagent ${SOURCE_FILES})
add_library( jpegwrapper-static  jpeg_mem.cpp j2k_mem.cpp)
add_executable( testCImg EXCLUDE_FROM_ALL testCImg.cpp )

##指定使用openjpeg静态库
add_definitions(-DOPJ_STATIC)
target_link_libraries(testCImg jpegwrapper-static ${JPEG_LIBRARY_STATIC} ${OPENJP2_LIBRARY_STATIC} )

install(TARGETS jpegwrapper-static
			RUNTIME DESTINATION bin
		  LIBRARY DESTINATION lib
      ARCHIVE DESTINATION lib/static)
file(GLOB_RECURSE wrapper_headers ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
install(FILES ${wrapper_headers} DESTINATION include)
