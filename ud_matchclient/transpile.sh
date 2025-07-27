#!/bin/bash

source ../ud_pyenv/bin/activate

rm -rf ../ud_webserver/match/static/match/game_files/*

em++ -O0 *.cpp -std=c++23 -sGL_DEBUG=1 -sMODULARIZE=1 -sNO_EXIT_RUNTIME=1 -sMAX_WEBGL_VERSION=2 -sEXPORTED_RUNTIME_METHODS=ccall,cwrap --preload-file resources/ -o game_client.js

cp *.js ../ud_webserver/match/static/match/game_files/
cp *.wasm ../ud_webserver/match/static/match/game_files/
cp *.data ../ud_webserver/match/static/match/game_files/
