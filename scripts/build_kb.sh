#!/bin/bash
cd ..
REPO_PATH_FILE="repo.path"
PREPARED_KB="prepared_kb"

ERRORS_FILE="${PWD}/prepare.log"
if [[ -e  ${ERRORS_FILE} ]]; then
    rm "$ERRORS_FILE"
fi
touch "$ERRORS_FILE"

export LD_LIBRARY_PATH=./bin
if [ ! -d "./kb.bin" ]; then
    mkdir ./kb.bin
fi

python3 scripts/kb_scripts/prepare_kb.py "${PWD%/[^/]*}" $PREPARED_KB $REPO_PATH_FILE "$ERRORS_FILE"


if [[ -f ${ERRORS_FILE} && ! ( -s ${ERRORS_FILE} )]]; then
  ./bin/sc-builder -f -c -i $PREPARED_KB/$REPO_PATH_FILE -o kb.bin -s config/sc-web.ini -e ./bin/extensions
  rm "$ERRORS_FILE"
  rm prepare_kb
fi

