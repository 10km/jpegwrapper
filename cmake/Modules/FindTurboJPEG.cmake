#.rst:
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
#		turbojpep
# 	turbojpeg-static

find_path(TURBOJPEG_INCLUDE_DIR turbojpeg.h)

set(TURBOJPEG_NAMES ${TURBOJPEG_NAMES} turbojpeg libturbojpeg)
find_library(TURBOJPEG_LIBRARY NAMES ${TURBOJPEG_NAMES} )

# handle the QUIETLY and REQUIRED arguments and set TURBOJPEG_FOUND to TRUE if
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(TURBOJPEG DEFAULT_MSG TURBOJPEG_LIBRARY TURBOJPEG_INCLUDE_DIR)

if(TURBOJPEG_FOUND)
	# for compatility of find_dependency
	set (TurboJPEG_FOUND TRUE)
	# Create imported target turbojpeg
	add_library(turbojpep SHARED IMPORTED)
	set_target_properties(turbojpep PROPERTIES
	  IMPORTED_LINK_INTERFACE_LANGUAGES "C"
	  INTERFACE_INCLUDE_DIRECTORIES "${TURBOJPEG_INCLUDE_DIR}"
	  IMPORTED_LOCATION "${TURBOJPEG_LIBRARY}"
	  )
	if(MSVC)
		set(_stati_library_name ${CMAKE_STATIC_LIBRARY_PREFIX}turbojpeg-static${CMAKE_STATIC_LIBRARY_SUFFIX})
	else()
		set(_stati_library_name ${CMAKE_STATIC_LIBRARY_PREFIX}turbojpeg${CMAKE_STATIC_LIBRARY_SUFFIX})
	endif()
	find_library(TURBOJPEG_LIBRARY_STATIC ${_stati_library_name})
	if(TURBOJPEG_LIBRARY_STATIC)
		# Create imported target turbojpeg-static
		add_library(turbojpeg-static STATIC IMPORTED)
		set_target_properties(turbojpeg-static PROPERTIES
		  IMPORTED_LINK_INTERFACE_LANGUAGES "C"
		  INTERFACE_INCLUDE_DIRECTORIES "${TURBOJPEG_INCLUDE_DIR}"
		  IMPORTED_LOCATION "${TURBOJPEG_LIBRARY_STATIC}"
		  )
		message(STATUS "TURBOJPEG_LIBRARY_STATIC=${TURBOJPEG_LIBRARY_STATIC}")
	else()
		message(STATUS "NOT FOUND jpeg-turbo static library")
	endif()
	
	unset(_stati_library_name)
endif()

mark_as_advanced(TURBOJPEG_LIBRARY TURBOJPEG_INCLUDE_DIR )
