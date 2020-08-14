#ifndef SNAKE_H
#define SNAKE_H
#include <Arduino.h>
#include <CircuitOS.h>
#include <Input/I2cExpander.h>
#include <Input/InputI2C.h>
#include <Update/UpdateManager.h>
#include <ArduinoJson.h>
#include <spiffs_api.h>
#include <gpio.h>
#include <avr/pgmspace.h>
#include <Support/Context.h>
#include "../Nibble.hpp"
#include <UI/Image.h>

constexpr int BACKGROUND_COLOR = 0x0000; // Background color in hex. 0x0000 is black.
constexpr int SNAKE_SPEED_MIN = 1;       // Minimum movement in pixels per update. (value is inclusive)
constexpr int SNAKE_SPEED_MAX = 2;       // Maximum movement in pixels per update. (value is inclusive)
constexpr int EYE_COLOR = 0x0000;
constexpr int maxSnakeLength = 500;

extern Context* runningContext;

namespace Snake
{
    class Snake : public Context, public UpdateListener
    {
    public:
        Snake(Display &display);
        void draw() override;
        void start() override;
        void update(uint) override;
        void stop() override;

    private:
        Sprite *baseSprite;
        Input *buttons;
        String gamestatus;
        bool screenChange;
        static Snake *instance;
        String prevGamestatus = gamestatus;
        bool savePresent = 0;
        Display *display;
        bool speedB = 1;
        uint8_t menuSignal;
        bool borderFlag = 1;
        int8_t dirX;
        int8_t dirY;
        uint8_t xos;
        uint8_t yos;
        uint8_t snakeX[maxSnakeLength];
        uint8_t snakeY[maxSnakeLength];
        uint8_t foodX;
        uint8_t foodY;
        bool foodCoolFlag;
        uint16_t snakeLength;
        uint8_t tileSize = 5;
        bool snakeColor;
        bool eaten;
        uint16_t hScore;
        uint8_t foodSize = 4;
        uint8_t speed = 1;
        bool skip;
        uint8_t snakeColorVal;
        bool bigger;
		uint deadTime = 0;

		uint tempScore = 0;
		String name = "";
		String previous = "";
		uint8_t charCursor = 0;
		unsigned long elapsedMillis = millis();
		uint32_t hiscoreMillis = millis();
		bool hiscoreBlink = 0;
		uint32_t blinkMillis = millis();
		bool blinkState = 0;


        void titleSetup();
        void titleScreen();
        void clearButtonCallbacks();
        void drawFood();
        void drawSnake();
        void snakeMenu();
        void drawHead();
        void control();
        void setButtonCallbacksGame();
        void newGameSetup();
        void crash();
        void foodCheck();
        void dead();
        void oldgame();
        void paused();
        void pausedSetup();
        void snakeMenuControl();
		
		void enterInitialsSetup();
		void enterInitialsDraw();
		void enterInitialsUpdate();
		void eraseDataSetup();
		void eraseDataDraw();
		void eraseDataUpdate();
		void dataDisplaySetup();
		void dataDisplay();
    };
}; // namespace Snake

#endif