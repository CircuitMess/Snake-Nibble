#include "Snake.h"
#include <Audio/Piezo.h>
#include "Highscore.h"

Snake::Snake *Snake::Snake::instance = nullptr;
Snake::Snake::Snake(Display &display) : Context(display), baseSprite(display.getBaseSprite()),
										buttons(Input::getInstance()), display(&display)
{
	Serial.println(baseSprite->created() ? "created" : "not created");
	instance = this;
	gamestatus = "title";
	deadTime = 0;
	memset(snakeX, 0, sizeof(snakeX));
	memset(snakeY, 0, sizeof(snakeY));
	
}
void Snake::Snake::start()
{
	runningContext = this;
	Highscore.begin();
	prevGamestatus = "";
	draw();
	LoopManager::addListener(this);
}
void Snake::Snake::stop()
{
	clearButtonCallbacks();
	LoopManager::removeListener(this);
}
void Snake::Snake::pack(){
	exitingGame = true;
}
void Snake::Snake::draw()
{
	if (gamestatus == "title")
	{
		titleScreen();
		drawSnake();
	}
	if (gamestatus == "newgame")
	{
		oldgame();
	}
	if (gamestatus == "oldgame")
	{
		oldgame();
	}
	if(gamestatus == "dead")
	{
		if(deadTime <= 500000)
		{
			oldgame();
		}
		else
		{
			drawDead();
		}
	}
	if (gamestatus == "paused")
	{
		paused();
	}
	if(gamestatus == "eraseData")
	{
		eraseDataDraw();
	}
	if(gamestatus == "dataDisplay")
	{
		dataDisplay();
	}
	if(gamestatus == "enterInitials")
	{
		enterInitialsDraw();
	}
}
void Snake::Snake::titleScreen()
{
	baseSprite->clear(TFT_BLACK);
	baseSprite->setTextColor(TFT_GREEN);
	baseSprite->setCursor(25, 8);
	baseSprite->setTextFont(1);
	baseSprite->setTextSize(2);
	baseSprite->printCenter("SNAKE");
	baseSprite->setTextColor(TFT_LIGHTGREY);
	if (menuSignal == 0 && borderFlag == 1)
		baseSprite->setTextColor(TFT_YELLOW);
	baseSprite->setCursor(9, 32);
	baseSprite->setTextSize(2);
	baseSprite->print("WALL ");
	baseSprite->setCursor(60, 32);
	baseSprite->setTextColor(TFT_LIGHTGREY);
	if (menuSignal == 0 && borderFlag == 0)
		baseSprite->setTextColor(TFT_YELLOW);
	baseSprite->print(" FREE");
	baseSprite->setCursor(60, 55);
	baseSprite->setTextColor(TFT_LIGHTGREY);
	if (menuSignal == 1)
		baseSprite->setTextColor(TFT_YELLOW);
	baseSprite->printCenter("SPEED: ");
	baseSprite->setCursor(90, 55);
	baseSprite->print(instance->speed);
	baseSprite->setTextColor(TFT_LIGHTGREY);
	if (menuSignal == 2)
		baseSprite->setTextColor(TFT_YELLOW);
	baseSprite->setCursor(25, 78);
	baseSprite->printCenter("SCORES");
	baseSprite->setTextColor(TFT_LIGHTGREY);
	if (menuSignal == 3)
		baseSprite->setTextColor(TFT_YELLOW);
	baseSprite->setCursor(25, 101);
	baseSprite->printCenter("EXIT");
	/*if (speedB)
		speed = 1;
	else
		speed = 2;*/
}
void Snake::Snake::titleSetup()
{
	menuSignal = 0;
	clearButtonCallbacks();
	snakeMenu();
	buttons->setBtnPressCallback(BTN_UP, []() {
		if (instance->menuSignal > 0)
		{
			instance->menuSignal--;
		}
		else if (instance->menuSignal == 0)
		{
			instance->menuSignal = 3;
		}
		Piezo.tone(100, 100);
	});

	buttons->setBtnPressCallback(BTN_DOWN, []() {
		if (instance->menuSignal < 3)
		{
			instance->menuSignal++;
		}
		else if (instance->menuSignal == 3)
		{
			instance->menuSignal = 0;
		}
		Piezo.tone(100, 100);
	});

	buttons->setBtnPressCallback(BTN_RIGHT, []() {
		if (instance->menuSignal == 0)
		{
			instance->borderFlag = !instance->borderFlag;
		}
		if (instance->menuSignal == 1)
		{
			if (instance->speed < 3)
				instance->speed += 1;
			else
				instance->speed = 1;
		}
		Piezo.tone(100, 100);
	});
	buttons->setBtnPressCallback(BTN_LEFT, []() {
		if (instance->menuSignal == 0)
		{
			instance->borderFlag = !instance->borderFlag;
		}
		if (instance->menuSignal == 1)
		{
			if (instance->speed > 1)
				instance->speed -= 1;
			else
				instance->speed = 3;
		}
		Piezo.tone(100, 100);
	});

	buttons->setBtnPressCallback(BTN_A, []() {
		switch (instance->menuSignal)
		{
		case 0:
			instance->gamestatus = "newgame";
			break;
		case 1:
			instance->gamestatus = "newgame";
			break;
		case 2:
			instance->gamestatus = "dataDisplay";
			break;
		case 3:
			instance->pop();
			break;
		}
		Piezo.tone(200, 100);
	});
}
void Snake::Snake::drawHead()
{
	if (dirX == 1)
	{
		baseSprite->drawRect(snakeX[0] + 3, snakeY[0] + 1, 2, 3, TFT_RED);
		baseSprite->drawPixel(snakeX[0], snakeY[0] + 1, EYE_COLOR);
		baseSprite->drawPixel(snakeX[0] + 1, snakeY[0] + 1, TFT_BLACK);
		baseSprite->drawPixel(snakeX[0], snakeY[0] + 3, TFT_BLACK);
		baseSprite->drawPixel(snakeX[0] + 1, snakeY[0] + 3, TFT_BLACK);
	}
	else if (instance->dirX == -1)
	{
		baseSprite->fillRect(snakeX[0], snakeY[0] + 1, 2, 3, TFT_RED);
		baseSprite->drawPixel(snakeX[0] + 3, snakeY[0] + 1, TFT_BLACK);
		baseSprite->drawPixel(snakeX[0] + 3, snakeY[0] + 3, TFT_BLACK);
		baseSprite->drawPixel(snakeX[0] + 4, snakeY[0] + 1, TFT_BLACK);
		baseSprite->drawPixel(snakeX[0] + 4, snakeY[0] + 3, TFT_BLACK);
	}
	else if (instance->dirY == 1)
	{
		baseSprite->fillRect(snakeX[0] + 1, snakeY[0] + 3, 3, 2, TFT_RED);
		baseSprite->drawPixel(snakeX[0] + 1, snakeY[0], TFT_BLACK);
		baseSprite->drawPixel(snakeX[0] + 3, snakeY[0], TFT_BLACK);
		baseSprite->drawPixel(snakeX[0] + 1, snakeY[0] + 1, TFT_BLACK);
		baseSprite->drawPixel(snakeX[0] + 3, snakeY[0] + 1, TFT_BLACK);
	}
	else if (instance->dirY == -1)
	{
		baseSprite->fillRect(snakeX[0] + 1, snakeY[0], 3, 2, TFT_RED);
		baseSprite->drawPixel(snakeX[0] + 1, snakeY[0] + 3, TFT_BLACK);
		baseSprite->drawPixel(snakeX[0] + 3, snakeY[0] + 3, TFT_BLACK);
		baseSprite->drawPixel(snakeX[0] + 1, snakeY[0] + 4, TFT_BLACK);
		baseSprite->drawPixel(snakeX[0] + 3, snakeY[0] + 4, TFT_BLACK);
	}
}
void Snake::Snake::loop(uint _time)
{
	if (gamestatus != prevGamestatus)
	{
		screenChange = 1;
		prevGamestatus = gamestatus;
	}
	else
	{
		screenChange = 0;
	}
	if (gamestatus == "title")
	{
		if (screenChange)
		{
			titleSetup();
		}
		titleScreen();
		control();
		snakeMenuControl();
	}
	if (gamestatus == "newgame")
	{
		if (screenChange)
		{
			newGameSetup();
		}
		gamestatus = "oldgame";
	}
	if (gamestatus == "paused")
	{
		if (screenChange)
			pausedSetup();
		paused();
	}
	if (gamestatus == "oldgame")
	{
		if (screenChange)
		{
			setButtonCallbacksGame();
		}
		control();
		if (speed == 3){
			control();
		}
		crash();
		if (eaten){
			drawFood();
		}
		foodCheck();
	}

	if (gamestatus == "dead")
	{
		if(screenChange)
		{
			clearButtonCallbacks();
		}
		deadTime+=_time;
		dead();
	}
	if(gamestatus == "eraseData")
	{
		if(screenChange){
			eraseDataSetup();
		}
		eraseDataUpdate();
	}
	if(gamestatus == "dataDisplay")
	{
		if(screenChange){
			dataDisplaySetup();
		}
	}
	if(gamestatus == "enterInitials")
	{
		if(screenChange){
			enterInitialsSetup();
		}
		enterInitialsUpdate();
	}
	draw();
	display->commit();
}
void Snake::Snake::control()
{
	if (bigger)
	{
		for (int i = snakeLength - 6; i > 0; i--)
		{
			snakeX[i] = snakeX[i - 1];
			snakeY[i] = snakeY[i - 1];
		}
		for (int i = snakeLength; i > snakeLength - 6; i--)
		{
			snakeX[i] = snakeX[snakeLength - 7];
			snakeY[i] = snakeY[snakeLength - 7];
		}
		bigger = false;
	}
	else
	{
		for (int i = snakeLength; i > 0; i--)
		{
			snakeX[i] = snakeX[i - 1];
			snakeY[i] = snakeY[i - 1];
		}
	}
	snakeX[0] += instance->dirX;
	snakeY[0] += instance->dirY;
}
void Snake::Snake::clearButtonCallbacks()
{
	for (uint8_t i = 0; i < 6; i++)
	{
		buttons->setBtnReleaseCallback(i, nullptr);
		buttons->setBtnPressCallback(i, nullptr);
		buttons->setButtonHeldRepeatCallback(i, 0, nullptr);
		buttons->setButtonHeldCallback(i, 0, nullptr);
	}
}
void Snake::Snake::snakeMenu()
{
	memset(snakeX, 0, sizeof(snakeX));
	memset(snakeY, 0, sizeof(snakeY));
	dirX = 1; dirY = 0;
	snakeLength = 30;
}
void Snake::Snake::snakeMenuControl()
{
	if (snakeX[0] < 121 && snakeY[0] <= 1)
	{
		dirX = 1;
		dirY = 0;
	}
	else if (snakeX[0] >= 121 && snakeY[0] < 121)
	{
		dirX = 0;
		dirY = 1;
	}
	else if (snakeX[0] > 3 && snakeY[0] >= 121)
	{
		dirX = -1;
		dirY = 0;
	}
	else if (snakeX[0] <= 1 && snakeY[0] > 3)
	{
		dirX = 0;
		dirY = -1;
	}
}
void Snake::Snake::newGameSetup()
{
	deadTime = 0;
	setButtonCallbacksGame();
	for (int i = 400; i > 0; i--)
	{
		snakeX[i] = 128;
		snakeY[i] = 128;
		baseSprite->fillRect(snakeX[i], snakeY[i], tileSize, tileSize, TFT_BLACK);
	}
	snakeX[0] = 10;
	snakeY[0] = 63;
	foodX = random(3, 122);
	foodY = random(3, 122);

	dirX = 1 * 1;
	dirY = 0;
	snakeLength = 12;
	hScore = 0;
	gamestatus = "oldgame";
}
void Snake::Snake::drawFood()
{
	while (!foodCoolFlag)
	{
		foodX = random(3, 122);
		foodY = random(3, 122);
		for (uint8_t chX = 0; chX < 4; chX++)
		{
			for (uint8_t chY = 0; chY < 4; chY++)
			{
				if (baseSprite->readPixel(chX + foodX, chY + foodY) == 0)
				{
					foodCoolFlag = 1;
				}
				else
				{
					foodCoolFlag = 0;
					break;
				}
			}
			if (foodCoolFlag == 0)
				break;
		}
	}
	foodCoolFlag = 0;
	eaten = false;
}
void Snake::Snake::drawSnake()
{
	for (int i = (snakeLength)-1; i >= 0; i--)
	{
		baseSprite->fillRect(snakeX[i], snakeY[i], tileSize, tileSize, TFT_BLACK);
		if (i > (snakeLength)-10 && i < snakeLength)
			//snakeColor = false;
			snakeColorVal = 1;
		//else if(i < 4 || (i <= (snakeLength) - 6 &&  i % 2 == 1)) snakeColor = 1;
		else if (i >= 0 && i <= (snakeLength - 10))
			//snakeColor = true;
			snakeColorVal = 2;
		else
			snakeColorVal = 3;
		/* if (!foodX == snakeX[i] && !foodY == snakeY[i]) drawFood(); */
		//else snakeColor = 0;
		if (snakeColorVal == 2)
			baseSprite->fillRect(snakeX[i], snakeY[i], tileSize, tileSize, TFT_GREEN);
		else if (snakeColorVal == 1)
			baseSprite->fillRect(snakeX[i], snakeY[i], tileSize, tileSize, TFT_DARKGREEN);
		else
			baseSprite->fillRect(snakeX[i], snakeY[i], tileSize, tileSize, TFT_BLACK);
	}
	drawHead();
}
void Snake::Snake::setButtonCallbacksGame()
{
	clearButtonCallbacks();
	buttons->setBtnPressCallback(BTN_UP, []() {
		if (instance->dirY == 0)
		{
			instance->dirX = 0;
			instance->dirY = -1;
		}
	});
	buttons->setBtnPressCallback(BTN_DOWN, []() {
		if (instance->dirY == 0)
		{
			instance->dirX = 0;
			instance->dirY = 1;
		}
	});
	buttons->setBtnPressCallback(BTN_RIGHT, []() {
		if (instance->dirX == 0)
		{
			instance->dirX = 1;
			instance->dirY = 0;
		}
	});
	buttons->setBtnPressCallback(BTN_LEFT, []() {
		if (instance->dirX == 0)
		{
			instance->dirX = -1;
			instance->dirY = 0;
		}
	});
	buttons->setBtnPressCallback(BTN_B, []() {
		Serial.println("paused");
		instance->gamestatus = "paused";
		Piezo.tone(100, 100);
	});
}
void Snake::Snake::foodCheck()
{
	if (baseSprite->readPixel(snakeX[0], snakeY[0]) == TFT_YELLOW)
		eaten = true;
	else if (baseSprite->readPixel(snakeX[0] + 4, snakeY[0]) == TFT_YELLOW)
		eaten = true;
	else if (baseSprite->readPixel(snakeX[0], snakeY[0] + 4) == TFT_YELLOW)
		eaten = true;
	else if (baseSprite->readPixel(snakeX[0] + 4, snakeY[0] + 4) == TFT_YELLOW)
		eaten = true;
	else
		eaten = false;

	if (eaten)
	{
		snakeLength += 6;
		hScore += (1 * speed);
		Piezo.tone(200, 100);
		bigger = true;
	}
}
void Snake::Snake::crash()
{
	if (borderFlag)
	{
		if ((snakeX[0] <= 1 || snakeY[0] <= 1 || snakeX[0] >= 124 || snakeY[0] >= 124)){
			gamestatus = "dead";
			Piezo.tone(50, 300);
		}
	}
	else
	{
		for (int i = 0; i < snakeLength; i++)
		{
			if (snakeX[i] <= 0 && snakeX[i] > -8)
				snakeX[i] = 127;
			else if (snakeX[i] < -5)
				snakeX[i] = 127;
			else
				snakeX[i] = snakeX[i] % 128;
			if (snakeY[i] <= 0 && snakeY[i] > -8)
				snakeY[i] = 127;
			else if (snakeY[i] < -5)
				snakeY[i] = 127;
			else
				snakeY[i] = snakeY[i] % 128;
		}
	}
	// itself
	for (int i = 1; i < snakeLength; i++)
	{
		if (((snakeX[0] == snakeX[i + 10] || snakeX[0] == snakeX[i + 10] + 4) && (snakeY[0] == snakeY[i + 10] || snakeY[0] == snakeY[i + 10] + 4))
		|| ((snakeX[0] + 4 == snakeX[i + 10] || snakeX[0] + 4 == snakeX[i + 10] + 4) && (snakeY[0] + 4 == snakeY[i + 10] || snakeY[0] + 4 == snakeY[i + 10] + 4))){
			gamestatus = "dead";
			Piezo.tone(50, 300);
		}
	}
}
void Snake::Snake::drawDead()
{
	baseSprite->clear(TFT_BLACK);
	baseSprite->setTextSize(2);
	baseSprite->setTextFont(1);
	baseSprite->setCursor(5, 5);
	baseSprite->setTextColor(TFT_RED);
	baseSprite->printCenter("GAME OVER");
	baseSprite->setTextFont(2);
	baseSprite->setCursor(5, 55);
	baseSprite->setTextColor(TFT_YELLOW);
	baseSprite->setTextSize(1);
	baseSprite->printCenter("Your score:");
	baseSprite->println();
	baseSprite->setTextSize(2);
	baseSprite->printCenter(hScore);
}
void Snake::Snake::dead()
{
	if(deadTime > 2500000)
	{
		gamestatus = "enterInitials";
	}
}
void Snake::Snake::oldgame()
{
	if (speed > 1 || skip == 0)
	{
		baseSprite->clear(TFT_BLACK);
		if (borderFlag){
			baseSprite->drawRect(0, 0, 128, 128, TFT_WHITE);
		}
		baseSprite->setCursor(6, 110, 2);
		baseSprite->setTextSize(1);
		baseSprite->setTextColor(TFT_LIGHTGREY);
		baseSprite->print("SCORE:");
		baseSprite->print(hScore);
		if (eaten){
			baseSprite->fillRect(foodX, foodY, foodSize, foodSize, TFT_BLACK);
		}
		drawSnake();
		if (eaten){
			drawFood();
		}
		baseSprite->fillRect(foodX, foodY, foodSize, foodSize, TFT_YELLOW);
		skip = 1;
	}
	else{
		skip = 0;
	}
}
void Snake::Snake::pausedSetup()
{
	clearButtonCallbacks();
	buttons->setBtnPressCallback(BTN_B, []() {
		Serial.println("title");
		instance->gamestatus = "title";
		Piezo.tone(100, 100);
	});
	buttons->setBtnPressCallback(BTN_A, []() {
		Serial.println("oldgame");
		instance->gamestatus = "oldgame";
		Piezo.tone(200, 100);
	});
}
void Snake::Snake::paused()
{
	baseSprite->clear(TFT_BLACK);
	drawSnake();
	baseSprite->fillRect(foodX, foodY, foodSize, foodSize, TFT_YELLOW);
	if (borderFlag)
		baseSprite->drawRect(0, 0, 128, 128, TFT_WHITE);
	baseSprite->setTextFont(2);
	baseSprite->setCursor(6, 110);
	baseSprite->setTextSize(1);
	baseSprite->setTextColor(TFT_LIGHTGREY);
	baseSprite->print("SCORE:");
	baseSprite->print(hScore);
	baseSprite->setTextColor(TFT_LIGHTGREY);
	baseSprite->setCursor(35, 35);
	baseSprite->setTextSize(2);
	baseSprite->printCenter("PAUSED");
	baseSprite->setCursor(34, 65);
	baseSprite->setTextSize(1);
	baseSprite->printCenter("Press A to play");
	baseSprite->setTextFont(2);
	baseSprite->setCursor(35, 80);
	baseSprite->printCenter("Press B to exit");
}

void Snake::Snake::enterInitialsSetup()
{
	tempScore = Highscore.get(0).score;
	name = "AAA";
	charCursor = 0;
	previous = "";
	elapsedMillis = millis();
	hiscoreMillis = millis();
	blinkState = 1;
	hiscoreBlink = 0;
	clearButtonCallbacks();
	buttons->setBtnPressCallback(BTN_UP,[](){
		instance->blinkState = 1;
		instance->elapsedMillis = millis();
		instance->name[instance->charCursor]++;
		// A-Z 0-9 :-? !-/ ' '
		if (instance->name[instance->charCursor] == '0') instance->name[instance->charCursor] = ' ';
		if (instance->name[instance->charCursor] == '!') instance->name[instance->charCursor] = 'A';
		if (instance->name[instance->charCursor] == '[') instance->name[instance->charCursor] = '0';
		if (instance->name[instance->charCursor] == '@') instance->name[instance->charCursor] = '!';
	});
	buttons->setButtonHeldRepeatCallback(BTN_UP, 200, [](uint){
		instance->blinkState = 1;
		instance->elapsedMillis = millis();
		instance->name[instance->charCursor]++;
		// A-Z 0-9 :-? !-/ ' '
		if (instance->name[instance->charCursor] == '0') instance->name[instance->charCursor] = ' ';
		if (instance->name[instance->charCursor] == '!') instance->name[instance->charCursor] = 'A';
		if (instance->name[instance->charCursor] == '[') instance->name[instance->charCursor] = '0';
		if (instance->name[instance->charCursor] == '@') instance->name[instance->charCursor] = '!';
	});
	buttons->setButtonHeldRepeatCallback(BTN_DOWN, 200, [](uint){
		instance->blinkState = 1;
		instance->elapsedMillis = millis();
		instance->name[instance->charCursor]--;
		if (instance->name[instance->charCursor] == ' ') instance->name[instance->charCursor] = '?';
		if (instance->name[instance->charCursor] == '/') instance->name[instance->charCursor] = 'Z';
		if (instance->name[instance->charCursor] == 31)  instance->name[instance->charCursor] = '/';
		if (instance->name[instance->charCursor] == '@') instance->name[instance->charCursor] = ' ';
	});
	buttons->setBtnPressCallback(BTN_DOWN, [](){
		instance->blinkState = 1;
		instance->elapsedMillis = millis();
		instance->name[instance->charCursor]--;
		if (instance->name[instance->charCursor] == ' ') instance->name[instance->charCursor] = '?';
		if (instance->name[instance->charCursor] == '/') instance->name[instance->charCursor] = 'Z';
		if (instance->name[instance->charCursor] == 31)  instance->name[instance->charCursor] = '/';
		if (instance->name[instance->charCursor] == '@') instance->name[instance->charCursor] = ' ';
	});
	buttons->setBtnPressCallback(BTN_LEFT, [](){
		if(instance->charCursor > 0){
			instance->charCursor--;
			instance->blinkState = 1;
			instance->elapsedMillis = millis();
		}
	});
	buttons->setBtnPressCallback(BTN_RIGHT, [](){
		if(instance->charCursor < 2){
			instance->charCursor++;
			instance->blinkState = 1;
			instance->elapsedMillis = millis();
		}
	});
	buttons->setBtnPressCallback(BTN_A, [](){
		instance->charCursor++;
		instance->blinkState = 1;
		instance->elapsedMillis = millis();
	});
}
void Snake::Snake::enterInitialsUpdate() {

	if (millis() - elapsedMillis >= 350) //cursor blinking routine
	{
		elapsedMillis = millis();
		blinkState = !blinkState;
	}
	if(millis()-hiscoreMillis >= 1000)
	{
	hiscoreMillis = millis();
	hiscoreBlink = !hiscoreBlink;
	}
	previous = name;

	if (previous != name)
	{
	blinkState = 1;
	elapsedMillis = millis();
	}

	if(charCursor >= 3)
	{
		Score newScore;
		strcpy(newScore.name, name.c_str());
		newScore.score = hScore;
		Highscore.add(newScore);
		gamestatus = "dataDisplay";
	}
}
void Snake::Snake::enterInitialsDraw() {
	baseSprite->clear(TFT_BLACK);
	baseSprite->setCursor(16, 8);
	baseSprite->setTextFont(2);
	baseSprite->setTextColor(TFT_WHITE);
	baseSprite->setTextSize(1);
	baseSprite->printCenter("ENTER NAME");
	baseSprite->setCursor(20, 80);
	
	if(hiscoreBlink && hScore > tempScore)
	baseSprite->printCenter("NEW HIGH!");
	else
	baseSprite->printf("SCORE: %04d", hScore);

	baseSprite->setCursor(40, 40);
	baseSprite->print(name[0]);
	baseSprite->setCursor(55, 40);
	baseSprite->print(name[1]);
	baseSprite->setCursor(70, 40);
	baseSprite->print(name[2]);
	// baseSprite->drawRect(30, 38, 100, 20, TFT_WHITE);
	if(blinkState){
		baseSprite->drawFastHLine(38 + 15*charCursor, 56, 12, TFT_WHITE);
	}
}

void Snake::Snake::dataDisplaySetup()
{
	clearButtonCallbacks();
	buttons->setBtnPressCallback(BTN_UP, [](){
		instance->gamestatus = "eraseData";
	});
	buttons->setBtnPressCallback(BTN_A, [](){
		instance->gamestatus = "title";
	});
	buttons->setBtnPressCallback(BTN_B, [](){
		instance->gamestatus = "title";
	});
}
void Snake::Snake::dataDisplay()
{
	baseSprite->clear(TFT_BLACK);
	baseSprite->setCursor(32, -2);
	baseSprite->setTextSize(1);
	baseSprite->setTextFont(2);
	baseSprite->setTextColor(TFT_YELLOW);
	baseSprite->printCenter("HIGHSCORES");
	baseSprite->setTextColor(TFT_GREEN);
	baseSprite->setCursor(3, 110);
	for (int i = 1; i < 6;i++)
	{
		baseSprite->setCursor(6, i * 20);
		if(i <= Highscore.count())
		{
			Serial.printf("%d.   %.3s    %04d\n", i, Highscore.get(i - 1).name, Highscore.get(i - 1).score);
			Serial.println();
			baseSprite->printf("%d.   %.3s    %04d", i, Highscore.get(i - 1).name, Highscore.get(i - 1).score);
		}
		else
		{
			baseSprite->printf("%d.    ---   ----", i);
		}
	}
	Serial.println("---------------");
	baseSprite->setCursor(2, 115);
	baseSprite->print("Press UP to erase");
}

void Snake::Snake::eraseDataSetup()
{
	elapsedMillis = millis();
	blinkState = 1;
	clearButtonCallbacks();
	buttons->setBtnPressCallback(BTN_B, [](){
		instance->gamestatus = "dataDisplay";

	});
	buttons->setBtnPressCallback(BTN_A, [](){
		Highscore.clear();
		instance->gamestatus = "title";

	});
}
void Snake::Snake::eraseDataDraw()
{
	baseSprite->clear(TFT_BLACK);
	baseSprite->setTextFont(2);
	baseSprite->setTextColor(TFT_WHITE);
	baseSprite->setCursor(4, 5);
	baseSprite->printCenter("ARE YOU SURE?");
	baseSprite->setCursor(4, 25);
	baseSprite->printCenter("This cannot");
	baseSprite->setCursor(4, 41);
	baseSprite->printCenter("be reverted!");

	baseSprite->setCursor(10, 102);
	baseSprite->print("B:");
	baseSprite->setCursor(48, 102);
	baseSprite->print("Cancel");

	baseSprite->setCursor(10, 81);
	baseSprite->print("A:");

	if (blinkState){
		baseSprite->drawRect((baseSprite->width() - 60)/2 + 5, 80, 30*2, 9*2, TFT_RED);
		baseSprite->setTextColor(TFT_RED);
		baseSprite->setCursor(46, 81);
		baseSprite->print("DELETE");
	}
	else {
		baseSprite->fillRect((baseSprite->width() - 60)/2 + 5, 80, 30*2, 9*2, TFT_RED);
		baseSprite->setTextColor(TFT_WHITE);
		baseSprite->setCursor(46, 81);
		baseSprite->print("DELETE");
	}
}
void Snake::Snake::eraseDataUpdate()
{
	if (millis() - elapsedMillis >= 350) {
		elapsedMillis = millis();
		blinkState = !blinkState;
	}
}
