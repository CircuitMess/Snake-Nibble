#include "Snake.h"
//#include "sprites.hpp"
#include "icon.h"
Snake* Snake::instance = nullptr;
GameInfo Game::info = {"Snake", "Classic snake game", icon};
Snake::Snake(Display &display) : Game(display), baseSprite(display.getBaseSprite()),
                                 buttons(Input::getInstance()), display(&display)
{
    Serial.println("construcctor");
    delay(5);
    Serial.println(baseSprite->created() ? "created" : "not created");
    instance = this;
    highscoresPath = (char *)calloc(30, 1);
    strncpy(highscoresPath, "/", 30);
    if (info.title)
    {
        strncat(highscoresPath, info.title, 30);
    }
    else
    {
        strncat(highscoresPath, "game", 30);
    }
    strncat(highscoresPath, ".sav", 30);
}
void Snake::start()
{
    //randomSeed(millis() * micros());
    //baseSprite->clear(TFT_BLUE);
    prevGamestatus = "";
    gamestatus = "title";
    Serial.println(highscoresPath);
    File file = SPIFFS.open(highscoresPath, "r");
    deserializeJson(jb, file);
    JsonArray hiscores = jb.to<JsonArray>();
    file.close();
    if (!hiscores.isNull())
        savePresent = 1;
    else
    {
        Serial.println("No save present");
        JsonArray hiscores = jb.as<JsonArray>();
        File file = SPIFFS.open(highscoresPath, "w");
        serializeJson(hiscores, file);
        file.close();
    }
    serializeJsonPretty(hiscores, Serial);
    Serial.println("saves ok");
    draw();
    UpdateManager::addListener(this);
}
void Snake::stop()
{
   //clearButtonCallbacks();
    jb.clear();
    delete[] highscoresPath;
    UpdateManager::removeListener(this);
}

void Snake::draw()
{
    if (gamestatus == "title")
    {
        titleScreen();
        //drawSnake();
    }
    display->commit();
}

void Snake::titleScreen()
{
    baseSprite->clear(TFT_BLACK);
    baseSprite->setTextColor(TFT_GREEN);
    baseSprite->setCursor(25, 5);
    baseSprite->setTextFont(1);
    baseSprite->setTextSize(2);
    baseSprite->printCenter("SNAKE");
    baseSprite->setTextColor(TFT_LIGHTGREY);
    if(menuSignal == 0 && borderFlag == 1)baseSprite->setTextColor(TFT_YELLOW);
    baseSprite->setCursor(9, 30);
    baseSprite->setTextSize(2);
    baseSprite->print("WALL ");
    baseSprite->setCursor(60, 30);
    baseSprite->setTextColor(TFT_LIGHTGREY);
    if(menuSignal == 0 && borderFlag == 0)baseSprite->setTextColor(TFT_YELLOW);
    baseSprite->print(" FREE");
    baseSprite->setCursor(60, 53);
    baseSprite->setTextColor(TFT_LIGHTGREY);
    if(menuSignal == 1)baseSprite->setTextColor(TFT_YELLOW);
    baseSprite->printCenter("SPEED: ");
    baseSprite->setCursor(90, 53);
    baseSprite->print((instance->speed ? 1 : 2));
    baseSprite->setTextColor(TFT_LIGHTGREY);
    if(menuSignal == 2)baseSprite->setTextColor(TFT_YELLOW);
    baseSprite->setCursor(25, 76);
    baseSprite->printCenter("SCORES");
    baseSprite->setTextColor(TFT_LIGHTGREY);
    if(menuSignal == 3)baseSprite->setTextColor(TFT_YELLOW);
    baseSprite->setCursor(25, 99);
    baseSprite->printCenter("EXIT");



}
void Snake::titleSetup()
{
    menuSignal = 0;
    clearButtonCallbacks();
    //snakeMenu();
    buttons->setBtnPressCallback(BTN_UP, [](){
        if(instance->menuSignal > 0){
            instance->menuSignal--;
        }
        else if(instance->menuSignal == 0){
            instance->menuSignal = 3;
        }
    });

    buttons->setBtnPressCallback(BTN_DOWN, [](){
        if(instance->menuSignal < 3){
            instance->menuSignal++;
        }
        else if(instance->menuSignal == 3){
            instance->menuSignal = 0;
        }
    });

    buttons->setBtnPressCallback(BTN_RIGHT, [](){
        if(instance->menuSignal == 0){
            instance->borderFlag = !instance->borderFlag;
        }
        if(instance->menuSignal == 1){
            instance->speed = !instance->speed;
        }
    });
    buttons->setBtnPressCallback(BTN_LEFT, [](){
        if(instance->menuSignal == 0){
            instance->borderFlag = !instance->borderFlag;
        }
        if(instance->menuSignal == 1){
            instance->speed = !instance->speed;
        }
    });

    buttons->setBtnPressCallback(BTN_A, [](){
        switch (instance->menuSignal)
        {
            case 0:
                instance->gamestatus="newgame";
                break;
            case 1:
                instance->gamestatus="newgame";
                break;
            case 2: 
                instance->gamestatus="score";
                break;
            case 3:
                instance->pop();
            break;
        }    
    });
}

void Snake::drawHead(){
  if(dirX == 1*speed)  {
  baseSprite->drawRect(snakeX[0]+3, snakeY[0]+1, 2, 3,TFT_RED);
    baseSprite->drawPixel(snakeX[0], snakeY[0]+1, EYE_COLOR);
  baseSprite->drawPixel(snakeX[0]+1, snakeY[0]+1,  EYE_COLOR);
    baseSprite->drawPixel(snakeX[0], snakeY[0]+3,  EYE_COLOR);
  baseSprite->drawPixel(snakeX[0]+1, snakeY[0]+3,  EYE_COLOR);
  }
  else if(dirX == -1*speed)  {
    baseSprite->fillRect(snakeX[0], snakeY[0]+1, 2, 3,TFT_RED);
    baseSprite->drawPixel(snakeX[0]+3, snakeY[0]+1,  EYE_COLOR);
    baseSprite->drawPixel(snakeX[0]+3, snakeY[0]+3,  EYE_COLOR);
    baseSprite->drawPixel(snakeX[0]+4, snakeY[0]+1,  EYE_COLOR);
    baseSprite->drawPixel(snakeX[0]+4, snakeY[0]+3,  EYE_COLOR);
  }
  else if(dirY == 1*speed) {
     baseSprite->fillRect(snakeX[0]+1, snakeY[0]+3, 3, 2,TFT_RED);
     baseSprite->drawPixel(snakeX[0]+1, snakeY[0],  EYE_COLOR);
     baseSprite->drawPixel(snakeX[0]+3, snakeY[0],  EYE_COLOR);
     baseSprite->drawPixel(snakeX[0]+1, snakeY[0]+1,  EYE_COLOR);
     baseSprite->drawPixel(snakeX[0]+3, snakeY[0]+1,  EYE_COLOR);
  }
  else if(dirY == -1*speed) {
    baseSprite->fillRect(snakeX[0]+1, snakeY[0], 3, 2,TFT_RED);
    baseSprite->drawPixel(snakeX[0]+1, snakeY[0]+3,  EYE_COLOR);
    baseSprite->drawPixel(snakeX[0]+3, snakeY[0]+3,  EYE_COLOR);
    baseSprite->drawPixel(snakeX[0]+1, snakeY[0]+4,  EYE_COLOR);
    baseSprite->drawPixel(snakeX[0]+3, snakeY[0]+4,  EYE_COLOR);
  }
}
void Snake::update(uint){
    if(gamestatus != prevGamestatus)
	{
		screenChange = 1;
		prevGamestatus = gamestatus;
	}
	else
	{
		screenChange = 0;
	}
    if (gamestatus == "title") {
		if(screenChange)
		{
			titleSetup();
		}
		titleScreen();
        //drawSnake();
	}
    if (gamestatus == "newgame") {}//newgame(); }
    if (gamestatus == "gameover") {}
    if (gamestatus == "paused") {}
    	if(gamestatus == "eraseData")
	{
		if(screenChange){
			//eraseDataSetup();
		}
	}
	if(gamestatus == "dataDisplay")
	{
		if(screenChange){
			//dataDisplaySetup();
		}
	}
	if(gamestatus == "enterInitials")
	{
		if(screenChange){
			//enterInitialsSetup();
		}
	}
    draw();
}

void Snake::clearButtonCallbacks()
{
	for(uint8_t i = 0; i < 7; i++)
	{
		buttons->setBtnReleaseCallback(i, nullptr);
		buttons->setBtnPressCallback(i, nullptr);
		buttons->setButtonHeldRepeatCallback(i, 0, nullptr);
		buttons->setButtonHeldCallback(i, 0, nullptr);
	}
}
/*
void Snake::snakeMenu(){
    if(snakeMenuInt == 0) {
        snakeX[0] = 20;
        snakeY[0] = 1;
        snakeLength = 20;
        snakeMenuInt = 1;
    }
  if(snakeX[0] < 121 && snakeY[0]<=2){
    dirX = 1*speed;
    dirY = 0;
  }  
  else if(snakeX[0] >= 121 && snakeY[0] < 121){
    dirX = 0;
    dirY = 1*speed;
  } 
  else if(snakeX[0] > 3 && snakeY[0] >= 121){
    dirX = -1*speed;
    dirY = 0;
  } 
  else if(snakeX[0] <= 2 && snakeY[0]>3){
    dirX = 0;
    dirY = -1*speed;
  } 
}
*/

void Snake::drawFood(){
    while(!foodCoolFlag){
    foodX = random(3, 122);
    foodY = random(3, 122);
    for(uint8_t chX = 0; chX < 4; chX++){
        for(uint8_t chY = 0; chY < 4; chY++){
            if(baseSprite->readPixel(chX+foodX, chY+foodY) == 0){
                foodCoolFlag = 1;
            }
            else{
                foodCoolFlag = 0;
                break;
            }
        }
        if(foodCoolFlag == 0) break;
        }
    }
    foodCoolFlag = 0;
    eaten = false;
}

void Snake::drawSnake(){
for (int i = (snakeLength) - 1; i >= 0; i--){
   if ( i > (snakeLength) - 5) snakeColor = false;   
  //else if(i < 4 || (i <= (snakeLength) - 6 &&  i % 2 == 1)) snakeColor = 1;
  else snakeColor = true; 
  /* if (!foodX == snakeX[i] && !foodY == snakeY[i]) drawFood(); */
    //else snakeColor = 0;
  if(snakeColor == false) baseSprite->fillRect(snakeX[i], snakeY[i], tileSize, tileSize,TFT_GREEN);
  else baseSprite->fillRect(snakeX[i], snakeY[i], tileSize, tileSize,TFT_GREEN);
  }
    drawHead();
}

