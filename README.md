# jpegwrapper

基于libjpeg-turbo,openjpeg 实现对jpeg,jpeg2000(j2k)图像的内存编解码(C++11封装)。


## 代码下载

    git clone --recursive https://gitee.com/l0km/jpegwrapper.git

这里必须加上`--recursive`参数，以确保同时下载做为git子模块加入项目的[common_source_cpp](https://gitee.com/l0km/common_source_cpp)项目

MSVC下编译为静态连接`/MT`,要求Visual Studio 2015

## 命令行编译

要求cmake (2.6以上版本)
### windows
windows CMD下执行[`build_msvc.bat`](build_msvc.bat)自动编译32/64位静态库，编译器要求Visual Studio 2015.

### linux/gcc

linux或gcc下的编译方式参照`build_msvc.bat`,下面以MinGW为例说明：

	# 创建编译文件夹
	mkdir build.gcc
	cd build.gcc
	# 生成编译64位代码的Makefile
	cmake -G "MinGW Makefiles" \
			-DCMAKE_BUILD_TYPE=RELEASE -DTARGET_PROCESSOR=x86_64 \ 
			-DCMAKE_INSTALL_PREFIX=../release/jpegwrapper_x86_64 \ 
			..
	# 编译项目并安装到CMAKE_INSTALL_PREFIX指定的位置
	make install
	# 编译测试代码，默认情况下make不编译测试代码
	make testCImg


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

### libjpeg-turbo version 1.4.2

download from http://sourceforge.net/projects/libjpeg-turbo/files/

official site http://libjpeg-turbo.virtualgl.org/

libjpeg-turbo-x.x.x.tar.gz为源代码,在跨平台编译时，如果没有指定平台的libjpeg-turbo-xxx-xx-xx文件夹，要自行编译libjpeg-turbo-x.x.x.tar.gz

### CImg version 1.6.8

download from http://cimg.eu/

### openjpeg version 2.1

download from http://www.openjpeg.org/

CMakeLists_for_openjpeg.txt是在openjpeg的CMakeLists.txt基础上添加了修改MSVC编译时使用静态运行库(/MT)代码。

在MSVC下编译openjpeg时,将此文件覆盖openjpeg源码中的CMakeLists.txt


