#!/bin/bash

HERE=$(cd $(dirname $0) && pwd)
REPO_DIR=$(cd $HERE && git rev-parse --show-toplevel)

REPO_NAME=$(basename $REPO_DIR)
SCRIPT_NAME=$(echo $0 | sed "s|.*/$REPO_NAME|$REPO_NAME|g")

echo $SCRIPT_NAME
echo "----------"
echo

set -eux


main() {

    SOURCE_DIR=$REPO_DIR
    BUILD_DIR=$REPO_DIR/.build
    INSTALL_DIR=$REPO_DIR/.install

    cmake \
        -G Ninja \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX=$INSTALL_DIR \
        -B $BUILD_DIR \
        -S $SOURCE_DIR

    cmake --build $BUILD_DIR
    cmake --install $BUILD_DIR
}

main "$@"
