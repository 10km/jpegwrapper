echo off 
echo build jpegwrapper by VS2015
if not defined VS140COMNTOOLS (
	echo vs2015 NOT FOUND.
)
echo vs2015 found.
where cmake
if errorlevel 1 (
	echo cmake NOT FOUND.
)
echo cmake found

:msvc_x86
echo build x86 
if exist build rmdir build /s/q
mkdir build
cd build
call "%VS140COMNTOOLS%..\..\vc/vcvarsall" x86
cmake -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=RELEASE -DTARGET_PROCESSOR=x86 -DCMAKE_INSTALL_PREFIX=../release/jpegwrapper_x86 ..
nmake install
cd ..

:msvc_x86_64
echo build x86_64 
if exist build rmdir build /s/q
mkdir build
cd build
call "%VS140COMNTOOLS%..\..\vc/vcvarsall" x86_amd64
cmake -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=RELEASE -DTARGET_PROCESSOR=x86_64 -DCMAKE_INSTALL_PREFIX=../release/jpegwrapper_x86_64 ..
nmake install
cd ..

rmdir build/s/q