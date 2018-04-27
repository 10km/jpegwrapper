#author:guyadong
#created:2016/01/16
############################################################################
cmake_minimum_required( VERSION 2.6 )
#cmake_policy(SET CMP0022 NEW)

include (CMakePackageConfigHelpers)
# define RUNTIME_INSTALL_DIR LIBRARY_INSTALL_DIR INCLUDE_INSTALL_DIR CONFIG_INSTALL_DIR
set (RUNTIME_INSTALL_DIR bin)
set (LIBRARY_INSTALL_DIR lib)
set (INCLUDE_INSTALL_DIR include)
if(WIN32 AND NOT CYGWIN)
  set (CONFIG_INSTALL_DIR  cmake)
else()
  set (CONFIG_INSTALL_DIR  ${LIBRARY_INSTALL_DIR}/cmake/jpegwrapper)
endif()
find_package(TurboJPEG REQUIRED)
message(STATUS "TURBOJPEG_LIBRARY=${TURBOJPEG_LIBRARY}")
message(STATUS "TURBOJPEG_INCLUDE_DIR=${TURBOJPEG_INCLUDE_DIR}")
## 优先使用静态库
if(TARGET turbojpeg-static)
	set (_turbojpeg_target "turbojpeg-static")
else()
	set (_turbojpeg_target "turbojpeg")
endif()
find_package(OpenJPEG REQUIRED)
message(STATUS "OPENJPEG_LIBRARIES=${OPENJPEG_LIBRARIES}")
message(STATUS "OPENJPEG_BUILD_SHARED_LIBS=${OPENJPEG_BUILD_SHARED_LIBS}")
message(STATUS "OPENJPEG_INCLUDE_DIRS=${OPENJPEG_INCLUDE_DIRS}")

# 是否编译测试程序选项
option(AUTO_BUILD_TEST "build test program" OFF)

##### define static library ######
set(SOURCE_FILES
	jpeg_mem.cpp 
	j2k_mem.cpp
	)
file(GLOB _public_headers ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

# 用于外部调用的公共头文件
list (APPEND _public_headers
    ${INCLUDE_CIMG}/CImg.h
    ${DEPENDENT_SOURCE_DIR}/raii.h
    ${DEPENDENT_SOURCE_DIR}/assert_macros.h    
    )
if(MSVC)
	list(APPEND _public_headers ${DEPENDENT_SOURCE_DIR}/dirent.h)
endif()
set(_target_static jpegwrapper-static)
add_library( ${_target_static} STATIC ${SOURCE_FILES})
if(NOT OPENJPEG_BUILD_SHARED_LIBS)
	##指定使用openjpeg静态库
	target_compile_definitions (${_target_static} PUBLIC OPJ_STATIC	)
endif()
target_include_directories (${_target_static} 
	PRIVATE ${DEPENDENT_SOURCE_DIR}
	INTERFACE "$<INSTALL_INTERFACE:include>"
	)
# MSVC Debug 编译时生成包含用于调试器的完整符号调试信息的 .obj 文件
# see also https://msdn.microsoft.com/zh-cn/library/958x11bc.aspx
target_compile_options(${_target_static} PRIVATE $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Debug>>:/Z7>)
target_link_libraries(${_target_static} 
	PUBLIC openjp2 ${_turbojpeg_target}
	)

set_target_properties (${_target_static}  PROPERTIES 
	PUBLIC_HEADER "${_public_headers}"
	VERSION ${PROJECT_VERSION} 
	SOVERSION ${PROJECT_VERSION_MAJOR}
	)
##### define testCImg #######
add_executable( testCImg testCImg.cpp )
if(NOT AUTO_BUILD_TEST)
	set_target_properties(testCImg PROPERTIES EXCLUDE_FROM_ALL TRUE)
endif()
target_link_libraries(testCImg common_source ${_target_static} cimg)

target_compile_options(testCImg PUBLIC $<$<CXX_COMPILER_ID:MSVC>:/wd4996>)

if(NOT ${PROJECT_BINARY_DIR} STREQUAL ${PROJECT_SOURCE_DIR}) 
	# 复制测试图像到${PROJECT_BINARY_DIR}
	file(RELATIVE_PATH _current_folder ${PROJECT_SOURCE_DIR} ${CMAKE_CURRENT_LIST_DIR})
	file(COPY ${CMAKE_CURRENT_LIST_DIR}/images DESTINATION ${PROJECT_BINARY_DIR}/${_current_folder})
	unset(_current_folder)
endif()

# ----------------------------------------------------------------------------
set (jpegwrapper_DEPENDENCY "find_dependency(TurboJPEG) 
find_dependency(OpenJPEG)
")
configure_package_config_file (${PROJECT_SOURCE_DIR}/cmake/config.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/jpegwrapper-config.cmake
  INSTALL_DESTINATION ${CONFIG_INSTALL_DIR}
  NO_CHECK_REQUIRED_COMPONENTS_MACRO)
write_basic_package_version_file (jpegwrapper-config-version.cmake VERSION
  ${PROJECT_VERSION} COMPATIBILITY SameMajorVersion)
################安装脚本#################
install(TARGETS ${_target_static} EXPORT jpegwrapper-targets
  RUNTIME DESTINATION ${RUNTIME_INSTALL_DIR}
  LIBRARY DESTINATION ${LIBRARY_INSTALL_DIR}
  ARCHIVE DESTINATION ${LIBRARY_INSTALL_DIR}
  PUBLIC_HEADER DESTINATION ${INCLUDE_INSTALL_DIR}
  )
install (FILES
  ${CMAKE_CURRENT_BINARY_DIR}/jpegwrapper-config.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/jpegwrapper-config-version.cmake
  DESTINATION ${CONFIG_INSTALL_DIR}
  )
install(EXPORT jpegwrapper-targets NAMESPACE gdface:: DESTINATION ${CONFIG_INSTALL_DIR})

if(AUTO_BUILD_TEST)
	install(TARGETS testCImg RUNTIME DESTINATION ${RUNTIME_INSTALL_DIR})
	install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/images DESTINATION ${RUNTIME_INSTALL_DIR})
endif()

unset(_target_static)
unset(_public_headers)
unset(_turbojpeg_target)