em++ -O3 -MJ ../compile_commands_em++.json *.cpp -std=c++20 -s GL_DEBUG=1 -sMODULARIZE=1 -sNO_EXIT_RUNTIME=1 -sMAX_WEBGL_VERSION=2 -sEXPORTED_RUNTIME_METHODS=ccall,cwrap
echo [ > ../compile_commands.json
cat ../compile_commands_em++.json >> ../compile_commands.json
echo ] >> ../compile_commands.json

cp *.js ../ud_webserver/match/static/match/game_files/
cp *.wasm ../ud_webserver/match/static/match/game_files/
