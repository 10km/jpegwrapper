@rem ����%%1���� ����DEBUG|RELEASE�汾,Ĭ��RELEASE
@echo off 
echo build jpegwrapper by MinGW
where gcc
if errorlevel 1 (
	echo MinGW/gcc NOT FOUND.
	exit -1
)
echo MinGW/gcc found.
where cmake
if errorlevel 1 (
	echo cmake NOT FOUND.
	echo download from http://cmake.org/ ,extract to disk 
	echo add installation path to environment variable PATH
	pause
	exit -1
)
echo cmake found.

set sh_folder=%dp0
rem �������İ汾����(DEBUG|RELEASE)
set build_type=RELEASE
rem ����������1Ϊ"DEBUG"(�����ִ�Сд)�����DEBUG�汾
if /I "%1" == "DEBUG" ( set build_type=DEBUG) 
echo build_type=%build_type%

rem �ж��Ƿ��ܱ���64λ����
gcc --version |findstr "sjlj seh"
if errorlevel 1 (
	echo unsupported x86_64 build
	)else call:gcc_x86_64

rem �ж��Ƿ��ܱ���32λ����
gcc --version |findstr "sjlj dwarf"
if errorlevel 1 (
	echo unsupported x86 build	
	)else call:gcc_x86

goto :end

:gcc_x86
echo build x86 use MinGW 
if exist build_gcc_x86 rmdir build_gcc_x86 /s/q
mkdir build_gcc_x86
pushd build_gcc_x86
cmake -G "MinGW Makefiles" ^
	-DCMAKE_BUILD_TYPE=%build_type% ^
	-DCMAKE_DEBUG_POSTFIX=_d ^
	-DCMAKE_CXX_FLAGS=-m32 ^
	-DCMAKE_INSTALL_PREFIX=../release/jpegwrapper-windows-gcc-x86 ..
make install -j8
popd
rmdir build_gcc_x86 /s/q
goto:eof

:gcc_x86_64
echo build x86_64 use MinGW 
if exist build_gcc_x86_64 rmdir build_gcc_x86_64 /s/q
mkdir build_gcc_x86_64
pushd build_gcc_x86_64
cmake -G "MinGW Makefiles" ^
	-DCMAKE_BUILD_TYPE=%build_type% ^
	-DCMAKE_DEBUG_POSTFIX=_d ^
	-DCMAKE_CXX_FLAGS=-m64 ^
	-DCMAKE_INSTALL_PREFIX=../release/jpegwrapper-windows-gcc-x86_64 ..
make install -j8
popd
rmdir build_gcc_x86_64/s/q
goto:eof

:end