#ifndef SNAKE_HIGHSCORE_H
#define SNAKE_HIGHSCORE_H

#include <Arduino.h>

#define HS_NAME_LIMIT 3 // player name character limit
#define HS_MAX_SCORES 5 // # of players in HS
#define HS_FILENAME "/Snake" // SPIFFS filename for the HS, max 31 char

namespace Snake
{

struct Score {
	char name[HS_NAME_LIMIT + 1];
	uint score;
};

class HighscoreImpl {
public:
	void begin();
	void add(const Score& score);
	const Score& get(uint8_t i);
	void clear();
	uint8_t count();

private:
	struct Data {
		uint8_t count;
		Score scores[HS_MAX_SCORES];
	} data;

	void save();
	void load();
};

extern HighscoreImpl Highscore;

}
#endif // SNAKE_HIGHSCORE_H