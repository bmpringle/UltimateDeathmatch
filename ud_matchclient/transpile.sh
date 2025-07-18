emcc -O3 *.cpp -std=c++17 -sMODULARIZE=1 -sNO_EXIT_RUNTIME=1 -sMAX_WEBGL_VERSION=2 -sEXPORTED_RUNTIME_METHODS=ccall,cwrap

cp *.js ../ud_webserver/match/static/match/game_files/
cp *.wasm ../ud_webserver/match/static/match/game_files/
