#ifndef INPUT_H
#define INPUT_H

#include <emscripten.h>
#include <string>
#include <map>
#include <cmath>
#include "client.h"

EM_BOOL keydown_callback(int eventType, const EmscriptenKeyboardEvent *e, void *userData);

EM_BOOL keyup_callback(int eventType, const EmscriptenKeyboardEvent *e, void *userData);

EM_BOOL mousemove_callback(int eventType, const EmscriptenMouseEvent *e, void *userData);
#endif
