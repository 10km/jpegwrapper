#!/bin/bash
echo build jpegwrapper 
GXX_PATH=
if [ `/usr/bin/g++ -dumpversion` != "5.2.0" ]
then
	if [ `/usr/local/bin/g++ -dumpversion` = "5.2.0" ]
	then
		GXX_PATH="-DCMAKE_CXX_COMPILER:FILEPATH=/usr/local/bin/g++ -DCMAKE_C_COMPILER:FILEPATH=/usr/local/bin/gcc -DCMAKE_BUILD_TYPE:STRING=RELEASE"
	else
		echo "g++ compiler required version 5.2.0"
		exit -1
	fi
fi
if [ -d build ]
then 
	rm -fr build/*
else 
	mkdir build
fi
pushd build

cmake -G "Unix Makefiles" $GXX_PATH -DCMAKE_BUILD_TYPE=RELEASE -DCMAKE_INSTALL_PREFIX=../release/jpegwrapper-linux-x86_64 ..
make install
popd

rm -fr build
