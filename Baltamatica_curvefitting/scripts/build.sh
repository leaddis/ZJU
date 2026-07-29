#!/bin/bash

if [[ "$1" == "ubuntu16.04" ]]; then
    export CC=/usr/local/bin/gcc
    export CXX=/usr/local/bin/g++
    export LD_LIBRARY_PATH=/usr/local/lib:/usr/local/lib64
fi

ls /mnt
echo "Unpacking BALTAM_CORE component ..."
mkdir -p deps/core

filename_core_os=/mnt/BALTAM_CORE-$BALTAM_CORE_VERSION-Linux-$1-core.zip
filename_dev_os=/mnt/BALTAM_CORE-$BALTAM_CORE_VERSION-Linux-$1-dev.zip
filename_core=/mnt/BALTAM_CORE-$BALTAM_CORE_VERSION-Linux-core.zip
filename_dev=/mnt/BALTAM_CORE-$BALTAM_CORE_VERSION-Linux-dev.zip

if [[ -f "${filename_core_os}" ]]; then
    echo " > ${filename_core_os}"
    unzip -q ${filename_core_os} -d deps/core/
else
    echo " > ${filename_core}"
    unzip -q ${filename_core} -d deps/core/
fi

if [[ -f "${filename_dev_os}" ]]; then
    echo " > ${filename_dev_os}"
    unzip -q ${filename_dev_os} -d deps/core/
else
    echo " > ${filename_dev}"
    unzip -q ${filename_dev} -d deps/core/
fi

echo "Compiling the code..."
mkdir -p "build" && cd "build"

cmake -DCMAKE_BUILD_TYPE=Release -DPACKAGE_NAME_SUFFIX="$1" ..
make -j4
echo "Compile complete."
make package
