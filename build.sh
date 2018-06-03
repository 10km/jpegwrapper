#!/bin/bash
# 根据$1参数 编译DEBUG|RELEASE版本,默认RELEASE
echo build jpegwrapper 
GXX_PATH=
if [ `/usr/bin/g++ -dumpversion` \< "4.8.0" ]
then
	if [ `/usr/local/bin/g++ -dumpversion` \> "4.8.0" ]
	then
		GXX_PATH="-DCMAKE_CXX_COMPILER:FILEPATH=/usr/local/bin/g++ -DCMAKE_C_COMPILER:FILEPATH=/usr/local/bin/gcc -DCMAKE_BUILD_TYPE:STRING=RELEASE"
	else
		echo "g++ compiler required version 4.8.0"
		exit -1
	fi
fi

sh_folder=$(dirname $(readlink -f $0))
pushd $sh_folder
# 定义编译的版本类型(DEBUG|RELEASE)
build_type=RELEASE
typeset -u arg1=$1
[ "$arg1" = "DEBUG" ] && build_type=$arg1
echo build_type=$build_type

[ -d build ] && rm -fr build
mkdir build
pushd build

cmake .. -G "Unix Makefiles" $GXX_PATH \
	-DCMAKE_BUILD_TYPE=$build_type \
	-DCMAKE_DEBUG_POSTFIX=_d \
	-DCMAKE_INSTALL_PREFIX=../release/jpegwrapper-linux-x86_64 
make install
popd

#rm -fr build
popd
