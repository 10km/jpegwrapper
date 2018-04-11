# jpegwrapper
基于libjpeg,openjpeg实现对jpeg,jpeg2000图像的内存编解码C++11封装。

libjpeg-turbo version 1.4.2

download from http://sourceforge.net/projects/libjpeg-turbo/files/

official site http://libjpeg-turbo.virtualgl.org/

libjpeg-turbo-x.x.x.tar.gz为源代码,在跨平台编译时，如果没有指定平台的libjpeg-turbo-xxx-xx-xx文件夹，要自行编译libjpeg-turbo-x.x.x.tar.gz

CImg version 1.6.8

download from http://cimg.eu/

CMakeLists_for_openjpeg.txt是在openjpeg的CMakeLists.txt基础上添加了修改MSVC编译时使用静态运行库(/MT)代码。

在MSVC下编译openjpeg时,将此文件覆盖openjpeg源码中的CMakeLists.txt