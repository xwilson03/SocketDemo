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

    TMP_DIR=$(mktemp -d)
    trap "rm -rf $TMP_DIR" EXIT

    cat .build/compile_commands.json \
      | jq '[.[] | select(.file | test("_deps") | not)]' \
    > $TMP_DIR/compile_commands.json

    FAILED=0

    cppcheck \
        --project=$TMP_DIR/compile_commands.json \
        --enable=all \
        --suppress="*:*_deps*" \
        --suppress=missingIncludeSystem \
        --error-exitcode=1 \
    || FAILED=1

    run-clang-tidy \
        -p $TMP_DIR \
    || FAILED=1

    if [[ $FAILED -ne 0 ]]; then
        exit 1
    fi
}

main "$@"
