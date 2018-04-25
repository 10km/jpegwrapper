# jpegwrapper

基于libjpeg-turbo,openjpeg 实现对jpeg,jpeg2000(j2k)图像的内存编解码(C++11封装)。


## 代码下载

    git clone --recursive https://gitee.com/l0km/jpegwrapper.git

这里必须加上`--recursive`参数，以确保同时下载做为git子模块加入项目的[common_source_cpp](https://gitee.com/l0km/common_source_cpp)项目

## 命令行编译

要求cmake (2.6以上版本)
### windows

Visual Studio 2015编译,windows CMD下执行[`build_msvc.bat`](build_msvc.bat)自动编译32/64位静态库(/MT /MD)，共计4个版本

示例：

	mkdir build
	push build
	# 编译64位代码
	call "%VS140COMNTOOLS%..\..\vc/vcvarsall" x86_amd64
	cmake -G "NMake Makefiles" ^
		-DCMAKE_BUILD_TYPE=RELEASE ^ 
		-DCMAKE_INSTALL_PREFIX=../release/jpegwrapper_windows_vc_x86_64 ..
	nmake install
	popd


使用MinGW编译，执行[build_gcc.bat](build_gcc.bat),生成32还是64位程序取决于MinGW编译器版本，对于`SJLJ`版本编译器可以同时生成32/64位库。

示例：

    mkdir build_gcc_x86_64
	pushd build_gcc_x86_64
	cmake -G "MinGW Makefiles" ^
		-DCMAKE_BUILD_TYPE=RELEASE ^
		-DCMAKE_C_FLAGS=-m64 ^
		-DCMAKE_INSTALL_PREFIX=../release/jpegwrapper-windows-gcc-x86_64 ..
	make install -j8
	popd

### linux/gcc

linux下的命令行编译：

	# 创建编译文件夹
	mkdir build.gcc
	cd build.gcc
	cmake -G "Unix Makefiles" \
			-DCMAKE_BUILD_TYPE=RELEASE \ 
			-DCMAKE_INSTALL_PREFIX=../release/jpegwrapper-linux-x86_64 \ 
			-DAUTO_BUILD_TEST=ON \
			..
	# AUTO_BUILD_TEST用于设定是否自动编译测试程序，默认为OFF
	# 编译项目并安装到CMAKE_INSTALL_PREFIX指定的位置
	make install
	# 编译测试代码，默认情况下make不编译测试代码
	make testCImg

linux下编译testCImg 需要libX11支持，如果没有，请安装,例如(CentOS)：

	sudo yum install -y libX11-devel

linux下编译脚本 [build.sh](build.sh),编译时可能需要根据自己linux系统的实际情况修改脚本。

生成的静态库在release文件夹下

## Visual Studio 2015

执行 [make_msvc_project.bat](make_msvc_project.bat)可以生成Visual Studio 2015工程

## 代码结构

核心代码很少：

[j2k_mem.cpp](jpegwrapper/jpeg_mem.cpp):jpeg图像内存读写实现

[j2k_mem.cpp](jpegwrapper/j2k_mem.cpp):jpeg2000图像内存读写实现

[CImgWrapper.h](jpegwrapper/CImgWrapper.h):基于CImg进一步对jpeg,j2k图像读写做便利化封装，因为CImg.h体积很大，所以使用该头文件，会导致编译时间较长

## 调用方式

参见附带的测试代码 [testCImg.cpp](jpegwrapper/testCImg.cpp)


## 第三方库

所有依赖的第三方库在 [dependencies](dependencies)文件夹下，默认已经为openjpeg,turbojpeg提供了windows下msvc和MinGW编译编所需要的所有类型的release版本.linux版本需要用户自己执行对应的编译脚本编译。

### libjpeg-turbo 

download from http://sourceforge.net/projects/libjpeg-turbo/files/

official site http://libjpeg-turbo.virtualgl.org/

libjpeg-turbo-x.x.x.zip为源代码

编译jpeg-turbo需要[NASM](https://www.nasm.us/)编译器支持

linux下的编译脚本 

[build_jpeg_turbo.sh](dependencies/build_jpeg_turbo.sh)

**NOte**:
为了确保在linux虚拟机下也能正确编译turbojpeg，上面的编译脚本中关闭了SIMD指令支持(`-DWITH_SIMD=OFF`)。

SIMD指令支持对提高jpeg编解码效率很有帮助，如果要编译支持SIMD指令的版本，请在物理机linux系统下打开SIMD编译开关(`-DWITH_SIMD=ON`)再执行编译脚本。

windows下编译脚本：

MinGW [build_jpeg_turbo_gcc.bat](dependencies/build_jpeg_turbo_gcc.bat)

VS2015 [build_jpeg_turbo_msvc.bat](dependencies/build_jpeg_turbo_msvc.bat)

### CImg version 1.6.8

download from http://cimg.eu/

### openjpeg 

download from http://www.openjpeg.org/

linux下的编译脚本 

[build_openjpeg.sh](dependencies/build_openjpeg.sh)

windows下编译脚本：

MinGW [build_openjpeg_gcc.bat](dependencies/build_openjpeg_gcc.bat)

VS2015 [build_openjpeg_msvc.bat](dependencies/build_openjpeg_msvc.bat)

## cmake引用jpegwrapper 库示例

cmake查找 jpegwrapper 库的示例：

	# CONFIG模式查找 jpegwrapper 依赖库
	# 需要在 CMAKE_MODULE_PATH 指定 FindTurboJPEG.cmake的位置，本例中的位置在项目根目录下/cmake/Modules
	# 需要在 CMAKE_PREFIX_PATH 指定 jpegwrapper以及其依赖库turbojpeg,openjpeg的安装位置
	find_package(jpegwrapper CONFIG REQUIRED)

cmake脚本中引用 jpegwrapper 库的示例：

	# 引用jpegwrapper静态库
	target_link_libraries(test_jpegwrapper gdface::jpegwrapper-static)
	# 增加 openjpeg include
	target_include_directories (test_jpegwrapper PUBLIC ${OPENJPEG_INCLUDE_DIRS})


cmake脚本中引用 jpegwrapper 库的完整示例参见 [test/CMakeLists.txt](test/CMakeLists.txt)

创建调用示例的VS2015工程:
	
	set sh_folder=%~dp0
	pushd %sh_folder%
	mkdir build
	cd build
	call "%VS140COMNTOOLS%..\..\vc/vcvarsall" x86_amd64
	rem 生成64位工程
	set sh_folder=%sh_folder:\=/%
	echo creating x86_64 Project for Visual Studio 2015 ...
	cmake -G "Visual Studio 14 2015 Win64" ^
		-DCMAKE_MODULE_PATH=%sh_folder%../cmake/Modules ^
		-DCMAKE_PREFIX_PATH=%sh_folder%../release/jpegwrapper-windows-vc-x86_64;^%sh_folder%../dependencies/libjpeg-turbo-windows-vc-x86_64;%sh_folder%../dependencies/openjpeg-windows-vc-x86_64 ^
		..
	REM CMAKE_MODULE_PATH指定cmake module脚本位置
	REM CMAKE_PREFIX_PATH指定jpegwrapper及其依赖库turbojpeg,openjpeg的安装位置

完整脚本参见 [test/make_msvc_project.bat](test/make_msvc_project.bat)
 
生成unix Makefile过程参见：
[test/make_unix_makefile.sh](test/make_unix_makefile.sh)