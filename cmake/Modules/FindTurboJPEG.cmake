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


find_path(TURBOJPEG_INCLUDE_DIR turbojpeg.h)

set(TURBOJPEG_NAMES ${TURBOJPEG_NAMES} turbojpeg libturbojpeg)
find_library(TURBOJPEG_LIBRARY NAMES ${TURBOJPEG_NAMES} )

# handle the QUIETLY and REQUIRED arguments and set TURBOJPEG_FOUND to TRUE if
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(TURBOJPEG DEFAULT_MSG TURBOJPEG_LIBRARY TURBOJPEG_INCLUDE_DIR)

if(TURBOJPEG_FOUND)
	if(MSVC)
		set(_stati_library_name ${CMAKE_STATIC_LIBRARY_PREFIX}turbojpeg-static${CMAKE_STATIC_LIBRARY_SUFFIX})
	else()
		set(_stati_library_name ${CMAKE_STATIC_LIBRARY_PREFIX}turbojpeg${CMAKE_STATIC_LIBRARY_SUFFIX})
	endif()
	find_library(TURBOJPEG_LIBRARY_STATIC ${_stati_library_name})
	if(TURBOJPEG_LIBRARY_STATIC)
		message(STATUS "TURBOJPEG_LIBRARY_STATIC=${TURBOJPEG_LIBRARY_STATIC}")
	else()
		message(STATUS "NOT FOUND jpeg-turbo static library")
	endif()
	unset(_stati_library_name)
endif()

mark_as_advanced(TURBOJPEG_LIBRARY TURBOJPEG_INCLUDE_DIR )