#include "Snake.h"
#include "snake_icon.h"
Snake::Snake *Snake::Snake::instance = nullptr;
Snake::Snake::Snake(Display &display) : Context(display), baseSprite(display.getBaseSprite()),
                                        buttons(Input::getInstance()), display(&display)
{
    Serial.println("construcctor");
    delay(5);
    Serial.println(baseSprite->created() ? "created" : "not created");
    instance = this;
    // highscoresPath = (char *)calloc(30, 1);
    // strncpy(highscoresPath, "/", 30);
    // if (info.title)
    // {
    //     strncat(highscoresPath, info.title, 30);
    // }
    // else
    // {
    //     strncat(highscoresPath, "game", 30);
    // }
    // strncat(highscoresPath, ".sav", 30);
}
void Snake::Snake::start()
{
    //randomSeed(millis() * micros());
    //baseSprite->clear(TFT_BLUE);
    prevGamestatus = "";
    gamestatus = "title";
    // Serial.println(highscoresPath);
    // File file = SPIFFS.open(highscoresPath, "r");
    // deserializeJson(jb, file);
    // JsonArray hiscores = jb.to<JsonArray>();
    // file.close();
    // if (!hiscores.isNull())
    //     savePresent = 1;
    // else
    // {
    //     Serial.println("No save present");
    //     JsonArray hiscores = jb.as<JsonArray>();
    //     File file = SPIFFS.open(highscoresPath, "w");
    //     serializeJson(hiscores, file);
    //     file.close();
    // }
    // serializeJsonPretty(hiscores, Serial);
    // Serial.println("saves ok");
    draw();
    UpdateManager::addListener(this);
}
void Snake::Snake::stop()
{
    //clearButtonCallbacks();
    // jb.clear();
    // delete[] highscoresPath;
    UpdateManager::removeListener(this);
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
    if (gamestatus == "paused")
    {
        paused();
    }
    display->commit();
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
        tone(BUZZ_PIN, 100, 100);
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
        tone(BUZZ_PIN, 100, 100);
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
        tone(BUZZ_PIN, 100, 100);
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
        tone(BUZZ_PIN, 100, 100);
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
            //instance->gamestatus = "score";
            break;
        case 3:
            instance->pop();
            break;
        }
        tone(BUZZ_PIN, 200, 100);
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
void Snake::Snake::update(uint)
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
        drawSnake();
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
        oldGame();
    }

    if (gamestatus == "dead")
    {
        if (screenChange)
        {
            dead();
            //gamestatus = "enterInitials"
        }
        else
            gamestatus = "title";
    }
    if (gamestatus == "eraseData")
    {
        if (screenChange)
        {
            //eraseDataSetup();
        }
    }
    if (gamestatus == "dataDisplay")
    {
        if (screenChange)
        {
            //dataDisplaySetup();
        }
    }
    if (gamestatus == "enterInitials")
    {
        if (screenChange)
        {
            //enterInitialsSetup();
        }
    }
    draw();
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
    for (uint8_t i = 0; i < 7; i++)
    {
        buttons->setBtnReleaseCallback(i, nullptr);
        buttons->setBtnPressCallback(i, nullptr);
        buttons->setButtonHeldRepeatCallback(i, 0, nullptr);
        buttons->setButtonHeldCallback(i, 0, nullptr);
    }
}
void Snake::Snake::snakeMenu()
{
    if (snakeMenuInt == 0)
    {
        snakeX[0] = 0;
        snakeY[0] = 0;
        snakeMenuInt = 1;
    }
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
    snakeLength = 12 / 1;
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
        tone(BUZZ_PIN, 100, 100);
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
        tone(BUZZ_PIN, 200, 100);
        bigger = true;
    }
}
void Snake::Snake::crash()
{
    if (borderFlag)
    {
        if ((snakeX[0] <= 1 || snakeY[0] <= 1 || snakeX[0] >= 124 || snakeY[0] >= 124))
            gamestatus = "dead";
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
        if (
            ((snakeX[0] == snakeX[i + 10] || snakeX[0] == snakeX[i + 10] + 4) && (snakeY[0] == snakeY[i + 10] || snakeY[0] == snakeY[i + 10] + 4)) || ((snakeX[0] + 4 == snakeX[i + 10] || snakeX[0] + 4 == snakeX[i + 10] + 4) && (snakeY[0] + 4 == snakeY[i + 10] || snakeY[0] + 4 == snakeY[i + 10] + 4)))
            gamestatus = "dead";
    }
}
void Snake::Snake::dead()
{
    delay(1000);
    tone(BUZZ_PIN, 50, 300);
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
    display->commit();
    delay(1500);
}
void Snake::Snake::oldgame()
{
    if (speed > 1 || skip == 0)
    {
        baseSprite->clear(TFT_BLACK);
        if (borderFlag)
            baseSprite->drawRect(0, 0, 128, 128, TFT_WHITE);
        baseSprite->setCursor(6, 110, 2);
        baseSprite->setTextSize(1);
        baseSprite->setTextColor(TFT_LIGHTGREY);
        baseSprite->print("SCORE:");
        baseSprite->print(hScore);
        control();
        if (speed == 3)
            control();
        crash();
        if (eaten)
            baseSprite->fillRect(foodX, foodY, foodSize, foodSize, TFT_BLACK);
        drawSnake();
        if (eaten)
            drawFood();
        baseSprite->fillRect(foodX, foodY, foodSize, foodSize, TFT_YELLOW);
        foodCheck();

        skip = 1;
    }
    else
        skip = 0;
}
void Snake::Snake::pausedSetup()
{
    clearButtonCallbacks();
    buttons->setBtnPressCallback(BTN_B, []() {
        Serial.println("title");
        instance->gamestatus = "title";
        tone(BUZZ_PIN, 100, 100);
    });
    buttons->setBtnPressCallback(BTN_A, []() {
        Serial.println("oldgame");
        instance->gamestatus = "oldgame";
        tone(BUZZ_PIN, 200, 100);
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
void Snake::Snake::oldGame() {}