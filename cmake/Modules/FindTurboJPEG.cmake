# FindTURBOJPEG
# --------
#
# Find Turbo JPEG
#
# Find the native Turbo JPEG includes and library This module defines
#
# ::
#
#   TURBOJPEG_INCLUDE_DIR, where to find turbojpeg.h, etc.
#   TURBOJPEG_FOUND, If false, do not try to use Turbo JPEG.
#   TURBOJPEG_LIBRARY, where to find the Turbo JPEG library.
#   TURBOJPEG_LIBRARY_STATIC where to find the Turbo JPEG static library
# import target:
#		turbojpeg
# 	turbojpeg-static

find_path(TURBOJPEG_INCLUDE_DIR turbojpeg.h)

set(TURBOJPEG_NAMES ${TURBOJPEG_NAMES} turbojpeg libturbojpeg)
find_library(TURBOJPEG_LIBRARY NAMES ${TURBOJPEG_NAMES} )

# handle the QUIETLY and REQUIRED arguments and set TURBOJPEG_FOUND to TRUE if
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(TURBOJPEG DEFAULT_MSG TURBOJPEG_LIBRARY TURBOJPEG_INCLUDE_DIR)
# 根据已有的库lib_var,查找debug版库,
# 创建变量${lib_var}_DEBUG保存结果,没有找到则值为${lib_var}_DEBUG-NOTFOUND
macro(debug_library lib_var)
	if(NOT DEFINED ${lib_var})
		message(FATAL_ERROR "not defined lib_var:${lib_var}")
	endif()
	if(NOT EXISTS "${${lib_var}}")
		message(FATAL_ERROR "not exists file:${${lib_var}}")
	endif()	
	get_filename_component(_name ${${lib_var}} NAME_WE)
	get_filename_component(_ext ${${lib_var}} EXT)
	get_filename_component(_path ${${lib_var}} PATH)
	find_file(${lib_var}_DEBUG ${_name}_d${_ext} 
		PATHS ${_path} 
		NO_DEFAULT_PATH)
	unset(_name)
	unset(_ext)
	unset(_path)
endmacro()
if(TURBOJPEG_FOUND)
	# for compatility of find_dependency
	set (TurboJPEG_FOUND TRUE)
	# Create imported target turbojpeg
	add_library(turbojpeg SHARED IMPORTED)
	# Import target "turbojpeg" for configuration "RELEASE"
	set_property(TARGET turbojpeg APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
	set_target_properties(turbojpeg PROPERTIES
	  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
	  INTERFACE_INCLUDE_DIRECTORIES "${TURBOJPEG_INCLUDE_DIR}"
	  IMPORTED_LOCATION_RELEASE "${TURBOJPEG_LIBRARY}"
	  )
	debug_library(TURBOJPEG_LIBRARY)
	if(TURBOJPEG_LIBRARY_DEBUG)
		# Import target "turbojpeg" for configuration "DEBUG"
		set_property(TARGET turbojpeg APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
		set_target_properties(turbojpeg PROPERTIES
		  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "C"
		  IMPORTED_LOCATION_DEBUG "${TURBOJPEG_LIBRARY_DEBUG}"
		  )
		 message(STATUS "deubug library ${TURBOJPEG_LIBRARY_DEBUG}")
	endif()
		
	if(MSVC)
		set(_stati_library_name ${CMAKE_STATIC_LIBRARY_PREFIX}turbojpeg-static${CMAKE_STATIC_LIBRARY_SUFFIX})
	else()
		set(_stati_library_name ${CMAKE_STATIC_LIBRARY_PREFIX}turbojpeg${CMAKE_STATIC_LIBRARY_SUFFIX})
	endif()
	find_library(TURBOJPEG_LIBRARY_STATIC ${_stati_library_name})
	if(TURBOJPEG_LIBRARY_STATIC)
		# Create imported target turbojpeg-static
		add_library(turbojpeg-static STATIC IMPORTED)
		# Import target "turbojpeg-static" for configuration "RELEASE"
		set_property(TARGET turbojpeg-static APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
		set_target_properties(turbojpeg-static PROPERTIES
		  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
		  INTERFACE_INCLUDE_DIRECTORIES "${TURBOJPEG_INCLUDE_DIR}"
		  IMPORTED_LOCATION_RELEASE "${TURBOJPEG_LIBRARY_STATIC}"
		  )
		debug_library(TURBOJPEG_LIBRARY_STATIC)
		if(TURBOJPEG_LIBRARY_STATIC_DEBUG)
			# Import target "turbojpeg-static" for configuration "DEBUG"
			set_property(TARGET turbojpeg-static APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
			set_target_properties(turbojpeg-static PROPERTIES
			  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "C"
			  IMPORTED_LOCATION_DEBUG "${TURBOJPEG_LIBRARY_STATIC_DEBUG}"
			  )
			message(STATUS "TURBOJPEG_LIBRARY_STATIC_DEBUG=${TURBOJPEG_LIBRARY_STATIC_DEBUG}")
		endif()
		message(STATUS "TURBOJPEG_LIBRARY_STATIC=${TURBOJPEG_LIBRARY_STATIC}")
	else()
		message(STATUS "NOT FOUND jpeg-turbo static library")
	endif()
	unset(_stati_library_name)
endif()

mark_as_advanced(TURBOJPEG_LIBRARY TURBOJPEG_INCLUDE_DIR )
