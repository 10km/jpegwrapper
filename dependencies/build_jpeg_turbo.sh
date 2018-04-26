#!/bin/bash
echo build jpeg-turbo by MinGW
which cmake
if [ ! $? -eq 0 ]
then
	echo cmake NOT FOUND.
	echo download from http://cmake.org/ ,extract to disk 
	echo add installation path to environment variable PATH
	exit -1
fi
echo cmake found.
which nasm
if [ ! $? -eq 0 ]
then
	echo "nasm NOT FOUND."
	echo "please install nasm,for example(CentOS): sudo yum install -y nasm  "
	echo "or download from https://www.nasm.us/ ,extract to disk add installation path to environment variable PATH"
	exit -1
fi
echo nasm found.
sh_folder=$(dirname $(readlink -f $0))
# 如果要编译Debug版本则改为DEBUG
build_type=DEBUG

build_gcc_x86(){
echo "build x86 use gcc"
if [ -d build_gcc_x86 ]
then 
	rm -fr build_gcc_x86/*
else 
	mkdir build_gcc_x86
fi
pushd build_gcc_x86

cmake -G "Unix Makefiles" \
	-DCMAKE_BUILD_TYPE=$build_type \
	-DCMAKE_DEBUG_POSTFIX=_d \
	-DCMAKE_C_FLAGS=-m32 \
	-DWITH_SIMD=OFF \
	-DCMAKE_INSTALL_PREFIX=$sh_folder/libjpeg-turbo-linux-x86 \
	..
make install -j8
popd
rm -fr build_gcc_x86
}

build_gcc_x86_64(){
#echo "build x86_64 use gcc"
if [ -d build_gcc_x86_64 ]
then 
	rm -fr build_gcc_x86_64/*
else 
	mkdir build_gcc_x86_64
fi

pushd build_gcc_x86_64

cmake -G "Unix Makefiles" \
	-DCMAKE_BUILD_TYPE=$build_type \
	-DCMAKE_DEBUG_POSTFIX=_d \
	-DCMAKE_C_FLAGS=-m64 \
	-DWITH_SIMD=OFF \
	-DCMAKE_INSTALL_PREFIX=$sh_folder/libjpeg-turbo-linux-x86_64 \
	..
make install -j8
popd
rm -fr build_gcc_x86_64
}
pushd $sh_folder
source_folder=libjpeg-turbo-1.5.90
if [ ! -d $source_folder ]
then 
	unzip $source_folder.zip
fi
pushd $source_folder

#build_gcc_x86
build_gcc_x86_64

popd
popd
