#include "HelloWorldScene.h"
#include "Scene2.h"
#include "ui/CocosGUI.h"

USING_NS_CC;

Scene* HelloWorld::createScene()
{
    return HelloWorld::create();
}

static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

bool HelloWorld::init()
{
    if (!Scene::init())
    {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    auto background = Sprite::create("background.jpg");
    if (!background) {
        log("Error loading background sprite.");
        return false;
    }
    background->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
    background->setContentSize(visibleSize);
    this->addChild(background, -1);

    auto menuItem1 = MenuItemImage::create(
        "button\\button_special.png",
        "button\\button_special_press.png",
        "button\\button_special_hover.png",
        CC_CALLBACK_1(HelloWorld::menuAction1Callback, this));

    auto menuItem2 = MenuItemImage::create(
        "button\\button_special.png",
        "button\\button_special_press.png",
        "button\\button_special_press.png",
        CC_CALLBACK_1(HelloWorld::menuAction2Callback, this));

    auto menuItem3 = MenuItemImage::create(
        "button\\button_special.png",
        "button\\button_special_press.png",
        "button\\button_special.png",
        CC_CALLBACK_1(HelloWorld::menuAction3Callback, this));

    auto menuItem4 = MenuItemImage::create(
        "button\\button_special.png",
        "button\\button_special_press.png",
        "button\\button_special.png",
        CC_CALLBACK_1(HelloWorld::menuAction4Callback, this));

    float scaleX = 1.2f;
    float scaleY = 0.7f;

    menuItem1->setScaleX(scaleX);
    menuItem1->setScaleY(scaleY);
    menuItem2->setScaleX(scaleX);
    menuItem2->setScaleY(scaleY);
    menuItem3->setScaleX(scaleX);
    menuItem3->setScaleY(scaleY);
    menuItem4->setScaleX(scaleX);
    menuItem4->setScaleY(scaleY);

    Size buttonSize = menuItem1->getContentSize();
    buttonSize.width *= scaleX;
    buttonSize.height *= scaleY;

    float spacing = 20;
    float totalHeight = (buttonSize.height + spacing) * 3;

    float startY = visibleSize.height / 2 + totalHeight / 2;

    menuItem1->setPosition(Vec2(visibleSize.width / 2, startY));
    menuItem2->setPosition(Vec2(visibleSize.width / 2, startY - (buttonSize.height + spacing)));
    menuItem3->setPosition(Vec2(visibleSize.width / 2, startY - 2 * (buttonSize.height + spacing)));
    menuItem4->setPosition(Vec2(visibleSize.width / 2, startY - 3 * (buttonSize.height + spacing)));

    auto menu = Menu::create(menuItem1, menuItem2, menuItem3, menuItem4, nullptr);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1, "menu");

    auto label1 = Label::createWithTTF("Play", "fonts/arial.ttf", 40);
    auto label2 = Label::createWithTTF("Score", "fonts/arial.ttf", 40);
    auto label3 = Label::createWithTTF("Option", "fonts/arial.ttf", 40);
    auto label4 = Label::createWithTTF("Quit", "fonts/arial.ttf", 40);

    if (label1 && label2 && label3 && label4) {
        label1->setPosition(menuItem1->getContentSize() / 2);
        label2->setPosition(menuItem2->getContentSize() / 2);
        label3->setPosition(menuItem3->getContentSize() / 2);
        label4->setPosition(menuItem4->getContentSize() / 2);

        menuItem1->addChild(label1);
        menuItem2->addChild(label2);
        menuItem3->addChild(label3);
        menuItem4->addChild(label4);
    }
    else {
        log("Error creating labels.");
    }

    return true;
}

void HelloWorld::menuAction1Callback(Ref* pSender)
{
    auto menu = this->getChildByName("menu");
    if (menu) {
        menu->setVisible(false);
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto origin = Director::getInstance()->getVisibleOrigin();

    auto loadingBackground = Sprite::create("loadingbar/loading_bar_empty.png");
    if (!loadingBackground) {
        log("Error loading loadingBackground sprite.");
        return;
    }
    loadingBackground->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
    this->addChild(loadingBackground);

    auto loadingSprite = Sprite::create("loadingbar/loading_fill.png");
    if (!loadingSprite) {
        log("Error loading loadingSprite sprite.");
        return;
    }
    auto loadingBar = ProgressTimer::create(loadingSprite);
    if (!loadingBar) {
        log("Error creating ProgressTimer.");
        return;
    }
    loadingBar->setType(ProgressTimer::Type::BAR);
    loadingBar->setBarChangeRate(Vec2(1, 0));
    loadingBar->setMidpoint(Vec2(0, 0));
    Size bgSize = loadingBackground->getContentSize();
    Size barSize = loadingSprite->getContentSize();

    float scaleX = (bgSize.width * 0.98f) / barSize.width;
    float scaleY = (bgSize.height * 1.0f) / barSize.height;
    float scale = std::min(scaleX, scaleY);

    loadingBar->setScaleX(scale);
    loadingBar->setScaleY(scale);

    Size scaledBarSize = loadingBar->getContentSize() * scale;
    Vec2 loadingBarPosition = Vec2(
        (bgSize.width - scaledBarSize.width) / 2 + scaledBarSize.width / 2,
        (bgSize.height - scaledBarSize.height) / 2 + scaledBarSize.height / 2
    );

    loadingBar->setPosition(loadingBarPosition);

    loadingBackground->addChild(loadingBar);

    auto progressTo = ProgressTo::create(2.0f, 100);
    auto replaceSceneAction = CallFunc::create([]() {
        auto s2 = Scene2::createScene();
        if (Director::getInstance()) {
            Director::getInstance()->replaceScene(s2);
        }
        });
    auto sequence = Sequence::create(progressTo, replaceSceneAction, nullptr);
    loadingBar->runAction(sequence);

    log("Button 1 pressed");
}

void HelloWorld::menuAction2Callback(Ref* pSender)
{
    auto menu = this->getChildByName("menu");
    if (menu) {
        menu->setVisible(false);
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto origin = Director::getInstance()->getVisibleOrigin();

    auto loadingBackground = Sprite::create("loadingbar/loading_bar_empty.png");
    if (!loadingBackground) {
        log("Error loading loadingBackground sprite.");
        return;
    }
    loadingBackground->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
    this->addChild(loadingBackground);

    auto loadingSprite = Sprite::create("loadingbar/loading_fill.png");
    if (!loadingSprite) {
        log("Error loading loadingSprite sprite.");
        return;
    }
    auto loadingBar = ProgressTimer::create(loadingSprite);
    if (!loadingBar) {
        log("Error creating ProgressTimer.");
        return;
    }
    loadingBar->setType(ProgressTimer::Type::BAR);
    loadingBar->setBarChangeRate(Vec2(1, 0));
    loadingBar->setMidpoint(Vec2(0, 0));
    Size bgSize = loadingBackground->getContentSize();
    Size barSize = loadingSprite->getContentSize();

    float scaleX = (bgSize.width * 0.98f) / barSize.width;
    float scaleY = (bgSize.height * 1.0f) / barSize.height;
    float scale = std::min(scaleX, scaleY);

    loadingBar->setScaleX(scale);
    loadingBar->setScaleY(scale);

    Size scaledBarSize = loadingBar->getContentSize() * scale;
    Vec2 loadingBarPosition = Vec2(
        (bgSize.width - scaledBarSize.width) / 2 + scaledBarSize.width / 2,
        (bgSize.height - scaledBarSize.height) / 2 + scaledBarSize.height / 2
    );

    loadingBar->setPosition(loadingBarPosition);

    loadingBackground->addChild(loadingBar);

    auto progressTo = ProgressTo::create(2.0f, 100);
    auto replaceSceneAction = CallFunc::create([]() {
        auto s2 = Scene2::createScene();
        if (Director::getInstance()) {
            Director::getInstance()->replaceScene(s2);
        }
        });
    auto sequence = Sequence::create(progressTo, replaceSceneAction, nullptr);
    loadingBar->runAction(sequence);

    log("Button 2 pressed");
}

void HelloWorld::menuAction3Callback(Ref* pSender)
{
    auto menu = this->getChildByName("menu");
    if (menu) {
        menu->setVisible(false);
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto origin = Director::getInstance()->getVisibleOrigin();

    auto loadingBackground = Sprite::create("loadingbar/loading_bar_empty.png");
    if (!loadingBackground) {
        log("Error loading loadingBackground sprite.");
        return;
    }
    loadingBackground->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
    this->addChild(loadingBackground);

    auto loadingSprite = Sprite::create("loadingbar/loading_fill.png");
    if (!loadingSprite) {
        log("Error loading loadingSprite sprite.");
        return;
    }
    auto loadingBar = ProgressTimer::create(loadingSprite);
    if (!loadingBar) {
        log("Error creating ProgressTimer.");
        return;
    }
    loadingBar->setType(ProgressTimer::Type::BAR);
    loadingBar->setBarChangeRate(Vec2(1, 0));
    loadingBar->setMidpoint(Vec2(0, 0));
    Size bgSize = loadingBackground->getContentSize();
    Size barSize = loadingSprite->getContentSize();

    float scaleX = (bgSize.width * 0.98f) / barSize.width;
    float scaleY = (bgSize.height * 1.0f) / barSize.height;
    float scale = std::min(scaleX, scaleY);

    loadingBar->setScaleX(scale);
    loadingBar->setScaleY(scale);

    Size scaledBarSize = loadingBar->getContentSize() * scale;
    Vec2 loadingBarPosition = Vec2(
        (bgSize.width - scaledBarSize.width) / 2 + scaledBarSize.width / 2,
        (bgSize.height - scaledBarSize.height) / 2 + scaledBarSize.height / 2
    );

    loadingBar->setPosition(loadingBarPosition);

    loadingBackground->addChild(loadingBar);

    auto progressTo = ProgressTo::create(2.0f, 100);
    auto replaceSceneAction = CallFunc::create([]() {
        auto s2 = Scene2::createScene();
        if (Director::getInstance()) {
            Director::getInstance()->replaceScene(s2);
        }
        });
    auto sequence = Sequence::create(progressTo, replaceSceneAction, nullptr);
    loadingBar->runAction(sequence);

    log("Button 3 pressed");
}

void HelloWorld::menuAction4Callback(Ref* pSender)
{
    auto menu = this->getChildByName("menu");
    if (menu) {
        menu->setVisible(false);
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto origin = Director::getInstance()->getVisibleOrigin();

    auto loadingBackground = Sprite::create("loadingbar/loading_bar_empty.png");
    if (!loadingBackground) {
        log("Error loading loadingBackground sprite.");
        return;
    }
    loadingBackground->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
    this->addChild(loadingBackground);

    auto loadingSprite = Sprite::create("loadingbar/loading_fill.png");
    if (!loadingSprite) {
        log("Error loading loadingSprite sprite.");
        return;
    }
    auto loadingBar = ProgressTimer::create(loadingSprite);
    if (!loadingBar) {
        log("Error creating ProgressTimer.");
        return;
    }
    loadingBar->setType(ProgressTimer::Type::BAR);
    loadingBar->setBarChangeRate(Vec2(1, 0));
    loadingBar->setMidpoint(Vec2(0, 0));
    Size bgSize = loadingBackground->getContentSize();
    Size barSize = loadingSprite->getContentSize();

    float scaleX = (bgSize.width * 0.98f) / barSize.width;
    float scaleY = (bgSize.height * 1.0f) / barSize.height;
    float scale = std::min(scaleX, scaleY);

    loadingBar->setScaleX(scale);
    loadingBar->setScaleY(scale);

    Size scaledBarSize = loadingBar->getContentSize() * scale;
    Vec2 loadingBarPosition = Vec2(
        (bgSize.width - scaledBarSize.width) / 2 + scaledBarSize.width / 2,
        (bgSize.height - scaledBarSize.height) / 2 + scaledBarSize.height / 2
    );

    loadingBar->setPosition(loadingBarPosition);

    loadingBackground->addChild(loadingBar);

    auto progressTo = ProgressTo::create(2.0f, 100);
    auto replaceSceneAction = CallFunc::create([]() {
        auto s2 = Scene2::createScene();
        if (Director::getInstance()) {
            Director::getInstance()->replaceScene(s2);
        }
        });
    auto sequence = Sequence::create(progressTo, replaceSceneAction, nullptr);
    loadingBar->runAction(sequence);

    log("Button 4 pressed");
}
