# jpegwrapper

基于libjpeg-turbo,openjpeg 实现对jpeg,jpeg2000(j2k)图像的内存编解码(C++11封装)。


## 代码下载

    git clone --recursive https://gitee.com/l0km/jpegwrapper.git

这里必须加上`--recursive`参数，以确保同时下载做为git子模块加入项目的[common_source_cpp](https://gitee.com/l0km/common_source_cpp)项目

MSVC下编译为静态连接`/MT`,要求Visual Studio 2015

## 命令行编译

要求cmake (2.6以上版本)
### windows

Visual Studio 2015编译,windows CMD下执行[`build_msvc.bat`](build_msvc.bat)自动编译32/64位静态库

	mkdir build
	push build
	call "%VS140COMNTOOLS%..\..\vc/vcvarsall" x86_amd64
	cmake -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=RELEASE -DCMAKE_INSTALL_PREFIX=../release/jpegwrapper_windows_vc_x86_64 ..
	nmake install
	popd


使用MinGW编译，执行[build_gcc.bat](build_gcc.bat),生成32还是64位程序取决于MinGW编译器版本

    mkdir build_gcc_x86_64
	pushd build_gcc_x86_64
	cmake -G "MinGW Makefiles" ^
		-DCMAKE_BUILD_TYPE=RELEASE ^
		-DCMAKE_C_FLAGS=-m64 ^
		-DCMAKE_INSTALL_PREFIX=../release/jpegwrapper_windows_gcc_x86_64 ..
	make install -j8
	popd

### linux/gcc

linux下的命令行编译：

	# 创建编译文件夹
	mkdir build.gcc
	cd build.gcc
	cmake -G "Unix Makefiles" \
			-DCMAKE_BUILD_TYPE=RELEASE \ 
			-DCMAKE_INSTALL_PREFIX=../release/jpegwrapper_linux_x86_64 \ 
			..
	# 编译项目并安装到CMAKE_INSTALL_PREFIX指定的位置
	make install
	# 编译测试代码，默认情况下make不编译测试代码
	make testCImg

linux下编译脚本 [build.sh](build.sh),编译时可能需要根据自己linux系统的实际情况修改脚本。

生成的静态库在release文件夹下

## Visual Studio 2015

如果你不会使用cmake，项目中也提供了msvc project,在windows下可以用Visual Studio 2015 打开工程文件 [jpegwrapper.sln](project.vs2015/jpegwrapper.sln)

## 代码结构

核心代码很少：

[j2k_mem.cpp](jpegwrapper/jpeg_mem.cpp):jpeg图像内存读写实现

[j2k_mem.cpp](jpegwrapper/j2k_mem.cpp):jpeg2000图像内存读写实现

[CImgWrapper.h](jpegwrapper/CImgWrapper.h):基于CImg进一步对jpeg,j2k图像读写做便利化封装，因为CImg.h体积很大，所以使用该头文件，会导致编译时间较长

## 调用方式

参见附带的测试代码 [testCImg.cpp](jpegwrapper/testCImg.cpp)


## 第三方库

所有依赖的第三方库在 [dependencies](dependencies)文件夹下

### libjpeg-turbo 

download from http://sourceforge.net/projects/libjpeg-turbo/files/

official site http://libjpeg-turbo.virtualgl.org/

libjpeg-turbo-x.x.x.zip为源代码

编译jpeg-turbo需要[NASM](https://www.nasm.us/)编译器支持

linux下的编译脚本 

[build_jpeg_turbo.sh](dependencies/build_jpeg_turbo.sh)


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


