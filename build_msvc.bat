@rem 根据%%1参数 编译DEBUG|RELEASE版本,默认RELEASE
@echo off 
echo build jpegwrapper by VS2015
if not defined VS140COMNTOOLS (
	echo vs2015 NOT FOUND.
	exit /B -1
)
echo vs2015 found.
where cmake
if errorlevel 1 (
	echo cmake NOT FOUND.
	exit /B -1
)
echo cmake found
set sh_folder=%~dp0
rem 定义编译的版本类型(DEBUG|RELEASE)
set build_type=RELEASE
rem 如果输入参数1为"DEBUG"(不区分大小写)则编译DEBUG版本
if /I "%1" == "DEBUG" ( set build_type=DEBUG) 
echo build_type=%build_type%

pushd %sh_folder%

call:msvc_x86
call:msvc_x86_64
goto :end

:msvc_x86
echo build x86 
if exist build rmdir build /s/q
mkdir build
pushd build
call "%VS140COMNTOOLS%..\..\vc/vcvarsall" x86
cmake -G "NMake Makefiles" ^
	-DCMAKE_BUILD_TYPE=%build_type% ^
	-DCMAKE_DEBUG_POSTFIX=_d ^
	-DCMAKE_INSTALL_PREFIX=../release/jpegwrapper-windows-vc-x86 ..
nmake install
del * /s/q
cmake -G "NMake Makefiles" ^
	-DCMAKE_BUILD_TYPE=%build_type% ^
	-DCMAKE_DEBUG_POSTFIX=_d ^
	-DCMAKE_USER_MAKE_RULES_OVERRIDE=%sh_folder%cmake\compiler_flag_overrides.cmake ^
	-DCMAKE_INSTALL_PREFIX=../release/jpegwrapper-windows-vc-x86-mt ..
nmake install

popd
goto:eof 

:msvc_x86_64
echo build x86_64 
if exist build rmdir build /s/q
mkdir build
pushd build
call "%VS140COMNTOOLS%..\..\vc/vcvarsall" x86_amd64
cmake -G "NMake Makefiles" ^
	-DCMAKE_BUILD_TYPE=%build_type% ^
	-DCMAKE_DEBUG_POSTFIX=_d ^
	-DCMAKE_INSTALL_PREFIX=../release/jpegwrapper-windows-vc-x86_64 ..
nmake install
del * /s/q
cmake -G "NMake Makefiles" ^
	-DCMAKE_BUILD_TYPE=%build_type% ^
	-DCMAKE_DEBUG_POSTFIX=_d ^
	-DCMAKE_USER_MAKE_RULES_OVERRIDE=%sh_folder%cmake\compiler_flag_overrides.cmake ^
	-DCMAKE_INSTALL_PREFIX=../release/jpegwrapper-windows-vc-x86_64-mt ..
nmake install

popd
rmdir build/s/q
goto:eof 

:end
popd