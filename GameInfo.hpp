#ifndef GAME_INFO
#define GAME_INFO
​ class Context;
struct GameInfo
{
	const char *title;
	const char *description;
	const uint16_t *icon;
	Context *(*launch)(Display &display);
};
#endif

#include <Arduino.h>
#include <Display/Display.h>
#include "src/snake_icon.hpp"
#include "src/Snake.h"
const GameInfo SnakeInfo
{
	"Snake",
		"Snake around and collect food to become bigger!",
		snake_icon,
		[](Display &display) -> Context * { return new Snake::Snake(display); }
};