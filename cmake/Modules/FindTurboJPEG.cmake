#.rst:
# FindTurboJPEG
# --------
#
# Find Turbo JPEG
#
# Find the native Turbo JPEG includes and library This module defines
#
# ::
#
#   TurboJPEG_INCLUDE_DIR, where to find jpeglib.h, etc.
#   TurboJPEG_LIBRARIES, the libraries needed to use Turbo JPEG.
#   TurboJPEG_FOUND, If false, do not try to use Turbo JPEG.
#   TurboJPEG_LIBRARY_STATIC Turbo JPEG static library
#
# also defined, but not for general use are
#
# ::
#
#   TurboJPEG_LIBRARY, where to find the Turbo JPEG library.


find_path(TurboJPEG_INCLUDE_DIR turbojpeg.h)

set(TurboJPEG_NAMES ${TurboJPEG_NAMES} turbojpeg libturbojpeg)
find_library(TurboJPEG_LIBRARY NAMES ${TurboJPEG_NAMES} )

# handle the QUIETLY and REQUIRED arguments and set TurboJPEG_FOUND to TRUE if
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(TurboJPEG DEFAULT_MSG TurboJPEG_LIBRARY TurboJPEG_INCLUDE_DIR)

if(TURBOJPEG_FOUND)
  set(TurboJPEG_LIBRARIES ${TurboJPEG_LIBRARY})

	if(MSVC)
		set(_stati_library_name ${CMAKE_STATIC_LIBRARY_PREFIX}turbojpeg-static${CMAKE_STATIC_LIBRARY_SUFFIX})
	else()
		set(_stati_library_name ${CMAKE_STATIC_LIBRARY_PREFIX}turbojpeg${CMAKE_STATIC_LIBRARY_SUFFIX})
	endif()
	find_library(TurboJPEG_LIBRARY_STATIC ${_stati_library_name})
	if(TurboJPEG_LIBRARY_STATIC)
		message(STATUS "TurboJPEG_LIBRARY_STATIC=${TurboJPEG_LIBRARY_STATIC}")
	else()
		message(STATUS "NOT FOUND jpeg-turbo static library")
	endif()
	unset(_stati_library_name)
endif()

mark_as_advanced(TurboJPEG_LIBRARY TurboJPEG_INCLUDE_DIR )
