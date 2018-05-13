echo off 
echo make test MinGW project
where gcc
if errorlevel 1 (
	echo MinGW/gcc NOT FOUND.
	exit /B -1
)
echo MinGW/gcc found.
where cmake
if errorlevel 1 (
	echo cmake NOT FOUND.
	exit /B -1
)
echo cmake found
set sh_folder=%~dp0
rem 删除最后的 '\'
set sh_folder=%sh_folder:~0,-1%
rem 获取所在文件夹名
for %%a in ("%sh_folder%") do set folder_name=%%~nxa
rem 定义编译的版本类型(DEBUG|RELEASE)
set build_type=DEBUG
rem 如果输入参数1为"RELEASE"(不区分大小写)则编译RELEASE版本
if /I "%1" == "RELEASE" ( set build_type=RELEASE)
if /I "%1" == "DEBUG" ( set build_type=DEBUG) 
echo build_type=%build_type%

pushd "%sh_folder%\.."
rem 判断是否能编译64位程序
gcc --version |findstr "sjlj seh"
if errorlevel 1 (
	echo unsupported x86_64 build
	paush 
	exit /B -1
	)
if exist "%folder_name%.mingw" rmdir "%folder_name%.mingw" /s/q
mkdir "%folder_name%.mingw"
pushd "%folder_name%.mingw"
set sh_folder=%sh_folder:\=/%
echo creating x86_64 Project for MinGW ...
cmake -G "Eclipse CDT4 - MinGW Makefiles" ^
	-DCMAKE_MODULE_PATH=%sh_folder%/../cmake/Modules ^
	-DCMAKE_BUILD_TYPE=%build_type% ^
	-DCMAKE_PREFIX_PATH=%sh_folder%/../release/jpegwrapper-windows-gcc-x86_64;%sh_folder%/../dependencies/libjpeg-turbo-windows-gcc-x86_64;%sh_folder%/../dependencies/openjpeg-windows-gcc-x86_64 ^
	%sh_folder%
	
popd
popd

goto :eof
