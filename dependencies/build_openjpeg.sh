#!/bin/bash
echo build openjpeg by gcc
which cmake
if [ ! $? -eq 0 ]
then
	echo "cmake NOT FOUND."
	echo "please install nasm,for example(CentOS): sudo yum install -y cmake3  "
	echo "or download from http://cmake.org/ ,extract to disk echo add installation path to environment variable PATH"
	exit -1
fi
echo cmake found.

sh_folder=$(dirname $(readlink -f $0))

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
	-DCMAKE_BUILD_TYPE=RELEASE \
	-DCMAKE_C_FLAGS=-m32 \
	-DCMAKE_INSTALL_PREFIX=$sh_folder/openjpeg-linux-x86 \
	-DBUILD_SHARED_LIBS=OFF \
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
	-DCMAKE_BUILD_TYPE=RELEASE \
	-DCMAKE_C_FLAGS=-m64 \
	-DCMAKE_INSTALL_PREFIX=$sh_folder/openjpeg-linux-x86_64 \
	-DBUILD_SHARED_LIBS=OFF \
	..
make install -j8
popd
rm -fr build_gcc_x86_64
} 
pushd $sh_folder
source_folder=openjpeg-version.2.1
if [ ! -d $source_folder ]
then
	unzip $source_folder.zip
fi
pushd $source_folder

#build_gcc_x86
build_gcc_x86_64

popd
popd
