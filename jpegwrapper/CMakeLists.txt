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

##### define static library ######
set(SOURCE_FILES
	jpeg_mem.cpp 
	j2k_mem.cpp
	)
# 用于外部调用的公共头文件
set (_PUBLIC_HEADERS
    ${DEPENDENT_SOURCE_DIR}/CImg.h
    ${DEPENDENT_SOURCE_DIR}/raii.h
    ${DEPENDENT_SOURCE_DIR}/assert_macros.h    
    )
if(MSVC)
	list(APPEND _PUBLIC_HEADERS ${DEPENDENT_SOURCE_DIR}/dirent.h)
endif()
set(_target_static jpegwrapper-static)
add_library( ${_target_static} STATIC ${SOURCE_FILES})
if(NOT OPENJPEG_BUILD_SHARED_LIBS)
	##指定使用openjpeg静态库
#	add_definitions(-DOPJ_STATIC)
	target_compile_definitions (${_target_static}
		PUBLIC OPJ_STATIC
		)
endif()
target_include_directories (${_target_static} 
	PRIVATE ${DEPENDENT_SOURCE_DIR}
	PUBLIC ${TURBOJPEG_INCLUDE_DIR}
	PUBLIC ${OPENJPEG_INCLUDE_DIRS}
	)

target_link_libraries(${_target_static} ${TURBOJPEG_LIBRARY} openjp2 )
##### define testCImg #######
add_executable( testCImg testCImg.cpp )
target_include_directories (testCImg  
	PRIVATE ${DEPENDENT_SOURCE_DIR} 
	)
target_link_libraries(testCImg ${_target_static} cimg)
target_compile_options(testCImg PUBLIC $<$<CXX_COMPILER_ID:MSVC>:/wd4996>)
install(TARGETS ${_target_static} testCImg
			RUNTIME DESTINATION bin
		  LIBRARY DESTINATION lib
      ARCHIVE DESTINATION lib/static)
install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/images DESTINATION bin)
file(GLOB wrapper_headers ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
install(FILES ${wrapper_headers} DESTINATION include)
install(FILES ${_PUBLIC_HEADERS} DESTINATION include)

unset(_target_static)
