#!/bin/bash

CGO_SRC="crt0/crt0.ssk"
CGO_TMP="./cgo"

cp "$CGO_SRC" "$CGO_TMP"
chmod +x "$CGO_TMP"

mkdir -p bin lib

"$CGO_TMP" src/varial.c -Iinclude -o bin/varial
"$CGO_TMP" -shared src/varial.c -Iinclude -o lib/libvarial.so

rm -f "$CGO_TMP"
