# FindOPENJPEG
# --------
#
# Find the OpenJPEG with CONFIG mode,and set target property INTERFACE_INCLUDE_DIRECTORIES if absent

find_package(openjpeg CONFIG)

if(TARGET openjp2)
	# �ж��Ƿ���INTERFACE_INCLUDE_DIRECTORIES�������û������Ӹ�����
	get_target_property(_include openjp2 INTERFACE_INCLUDE_DIRECTORIES)	
	if(_include)
		# do nothing		
	else()
		target_include_directories(openjp2 INTERFACE ${OPENJPEG_INCLUDE_DIRS})
		message(STATUS "add INTERFACE_INCLUDE_DIRECTORIES property for openjp2")
	endif()
	unset(_include)
endif()


