#!/bin/bash

VCPKG_DIR="./vcpkg"

if [ ! -d "$VCPKG_DIR" ]; then
    git clone https://github.com/microsoft/vcpkg.git $VCPKG_DIR
fi

cd $VCPKG_DIR
./bootstrap-vcpkg.sh

export VCPKG_HOME=$VCPKG_DIR

export PATH="$VCPKG_DIR:$PATH"

./vcpkg install boost-asio zlib
./vcpkg integrate install

echo "vcpkg setup complete"
