#include "Scene2.h"
#include "Box2D/Box2D.h"

namespace Common {
    const float PIXELS_PER_METER = 32.0f;
}

std::vector<b2Body*> bodiesToChange;

USING_NS_CC;

Scene* Scene2::createScene()
{
    auto scene = Scene::createWithPhysics();
    scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);

    auto layer = Scene2::create();
    layer->setPhysicworld(scene->getPhysicsWorld());
    scene->addChild(layer);

    return scene;
}

Sprite* platformSprite;

bool Scene2::init()
{
    if (!Scene::init()) {
        return false;
    }

    isCharacterOnGround = false;

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // Thiết lập trọng lực cho Box2D
    b2Vec2 gravity(0.0f, -9.8f);
    world = new b2World(gravity);
    world->SetContactListener(this);

    // Load bản đồ tiled map
    auto map = TMXTiledMap::create("ground1.tmx");
    if (map == nullptr) {
        log("Error loading map");
        return false;
    }

    auto groundobject = map->getObjectGroup("ground");

    Size mapSize = map->getContentSize();
    float scaleX = visibleSize.width / mapSize.width;
    float scaleY = visibleSize.height / mapSize.height;
    float scale = std::min(scaleX, scaleY); // Chọn tỷ lệ nhỏ hơn để giữ tỷ lệ khung hình

    for (const auto& obj : groundobject->getObjects()) {
        auto object = obj.asValueMap();

        // Lấy thông tin đối tượng và áp dụng tỷ lệ
        float x = object["x"].asFloat() / Common::PIXELS_PER_METER * scale;
        float y = object["y"].asFloat() / Common::PIXELS_PER_METER * scale;
        float width = object["width"].asFloat() / Common::PIXELS_PER_METER * scale;
        float height = object["height"].asFloat() / Common::PIXELS_PER_METER * scale;

        // Tạo body definition
        b2BodyDef bodyDef;
        bodyDef.type = b2_staticBody;
        bodyDef.position.Set(x + width / 2, y + height / 2);

        b2Body* body = world->CreateBody(&bodyDef);

        // Tạo shape cho mặt đất
        b2PolygonShape boxShape;
        boxShape.SetAsBox(width / 2, height / 2);

        // Tạo fixture definition
        b2FixtureDef fixtureDef;
        fixtureDef.shape = &boxShape;
        fixtureDef.density = 1.0f;
        fixtureDef.friction = 0.5f;
        fixtureDef.restitution = 0.0f;

        body->CreateFixture(&fixtureDef);
        log("Ground object Y position: %f", body->GetPosition().y);
    }

    auto ground_downobject = map->getObjectGroup("ground_down");
    for (const auto& obj : ground_downobject->getObjects()) {
        auto object = obj.asValueMap();

        float x = object["x"].asFloat() / Common::PIXELS_PER_METER * scale;
        float y = object["y"].asFloat() / Common::PIXELS_PER_METER * scale;
        float width = object["width"].asFloat() / Common::PIXELS_PER_METER * scale;
        float height = object["height"].asFloat() / Common::PIXELS_PER_METER * scale;

        b2BodyDef bodyDef;
        bodyDef.type = b2_staticBody;
        bodyDef.position.Set(x + width / 2, y + height / 2);

        b2Body* body = world->CreateBody(&bodyDef);

        b2PolygonShape boxShape;
        boxShape.SetAsBox(width / 2, height / 2);

        b2FixtureDef fixtureDef;
        fixtureDef.shape = &boxShape;
        fixtureDef.density = 1.0f;
        fixtureDef.friction = 0.5f;
        fixtureDef.restitution = 0.0f;

        body->CreateFixture(&fixtureDef);
        body->SetUserData((void*)"ground_down");
        log("Ground Down object Y position: %f", body->GetPosition().y);
        log("Ground Down object X position: %f", body->GetPosition().x);
    }

    map->setScale(scale);
    map->setPosition(Vec2(0, 0));
    this->addChild(map, 0, 99);

    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("character/sprites.plist");
    auto model = Sprite::createWithSpriteFrameName("idle1.png");
    if (!model) {
        return false;
    }
    model->setAnchorPoint(Vec2(0, 0));
    model->setScale(3);
    model->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));

    auto characterBatchNode = SpriteBatchNode::create("character/sprites.png");
    characterBatchNode->addChild(model);
    auto characterSpritebody = PhysicsBody::createBox(model->getContentSize(), PhysicsMaterial(1.0f, 0.3f, 0.7f));
    characterSpritebody->setMoment(PHYSICS_INFINITY);
    model->setPhysicsBody(characterSpritebody);
    this->addChild(characterBatchNode);

    auto animate = Animate::create(Scene2::createAnimation("idle", 10, 0.15));
    if (animate) {
        model->runAction(RepeatForever::create(animate));
    }

    auto characterSprite = Sprite::create("character/sprites.png");
    characterSprite->setScale(0.3);
    characterSprite->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));

    b2BodyDef charBodyDef;
    charBodyDef.type = b2_dynamicBody;
    charBodyDef.position.Set(
        characterSprite->getPositionX() / Common::PIXELS_PER_METER,
        characterSprite->getPositionY() / Common::PIXELS_PER_METER
    );

    charBody = world->CreateBody(&charBodyDef);

    b2PolygonShape charBox;
    charBox.SetAsBox(
        (characterSprite->getContentSize().width / 2 * characterSprite->getScaleX()) / Common::PIXELS_PER_METER,
        (characterSprite->getContentSize().height / 2 * characterSprite->getScaleY()) / Common::PIXELS_PER_METER
    );

    b2FixtureDef charFixtureDef;
    charFixtureDef.shape = &charBox;
    charFixtureDef.density = 1.0f;
    charFixtureDef.friction = 0.5f;
    charFixtureDef.restitution = 0.0f;

    charBody->CreateFixture(&charFixtureDef);

    characterSprite->setUserData(charBody);
    this->addChild(characterSprite);


    platformSprite = Sprite::create("ground_down2.jpg");
    platformSprite->setScale(scale - 1.11);

    float desiredWidth = 210.0f;
    Size originalSize = platformSprite->getContentSize();
    float newScaleX = desiredWidth / originalSize.width;

    platformSprite->setScaleX(newScaleX);
    platformSprite->setPosition(Vec2(30.833334 * Common::PIXELS_PER_METER, 6.266667 * Common::PIXELS_PER_METER));

    log("vi tri : %f %f", platformSprite->getPositionX(), platformSprite->getPositionY());

    b2BodyDef platformBodyDef;
    platformBodyDef.type = b2_staticBody;
    platformBodyDef.position.Set(platformSprite->getPositionX() / Common::PIXELS_PER_METER,
        platformSprite->getPositionY() / Common::PIXELS_PER_METER);

    b2Body* platformBody = world->CreateBody(&platformBodyDef);

    b2PolygonShape platformBox;
    platformBox.SetAsBox(
        (platformSprite->getContentSize().width * platformSprite->getScaleX()) / 2 / Common::PIXELS_PER_METER,
        (platformSprite->getContentSize().height * platformSprite->getScaleY()) / 2 / Common::PIXELS_PER_METER
    );

    b2FixtureDef platformFixtureDef;
    platformFixtureDef.shape = &platformBox;
    platformFixtureDef.density = 1.0f;
    platformFixtureDef.friction = 0.5f;
    platformFixtureDef.restitution = 0.0f;

    platformBody->CreateFixture(&platformFixtureDef);
    platformSprite->setUserData(platformBody);
    bodiesToChange.push_back(platformBody);

    this->addChild(platformSprite);

    log("vi tri : %f %f", platformSprite->getPositionX(), platformSprite->getPositionY());

    this->scheduleUpdate();

    auto listener = EventListenerKeyboard::create();
    listener->onKeyPressed = CC_CALLBACK_2(Scene2::onKeyPressed, this);
    listener->onKeyReleased = CC_CALLBACK_2(Scene2::onKeyReleased, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

    return true;
}


void Scene2::moveCharacter(float x, float y)
{
    if (charBody) {
        b2Vec2 velocity = charBody->GetLinearVelocity();
        velocity.x += x*10;
        charBody->SetLinearVelocity(velocity);
    }
}

void Scene2::onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event)
{
    switch (keyCode)
    {
    case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
        moveCharacter(-5.0f, 0.0f);
        break;
    case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
        moveCharacter(5.0f, 0.0f);
        break;
    case EventKeyboard::KeyCode::KEY_SPACE:
        float y = 0.0f;
        if (charBody) {
            b2Vec2 velocity = charBody->GetLinearVelocity();
            velocity.y += y + 50;
            charBody->SetLinearVelocity(velocity);
        }
    }
}

void Scene2::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
{

}

void Scene2::BeginContact(b2Contact* contact)
{
    b2Fixture* fixtureA = contact->GetFixtureA();
    b2Fixture* fixtureB = contact->GetFixtureB();

    b2Body* bodyA = fixtureA->GetBody();
    b2Body* bodyB = fixtureB->GetBody();

    log("BodyA type: %d, BodyB type: %d", bodyA->GetType(), bodyB->GetType());

    void* userDataA = bodyA->GetUserData();
    void* userDataB = bodyB->GetUserData();
    /*void* userDatac = (void*)"ground_down";
    log("Address of userDatac: %p", userDatac);*/

    log("UserDataA: %s, UserDataB: %p", userDataA, userDataB);

    if ((userDataA != nullptr && strcmp(static_cast<const char*>(userDataA), "ground_down") == 0 && bodyB->GetType() == b2_dynamicBody) ||
        (userDataB != nullptr && strcmp(static_cast<const char*>(userDataB), "ground_down") == 0 && bodyA->GetType() == b2_dynamicBody)) {
        b2Body* groundDownBody = (userDataA != nullptr && strcmp(static_cast<const char*>(userDataA), "ground_down") == 0) ? bodyA : bodyB;
        if (groundDownBody->GetType() == b2_staticBody) {
            log("cham ground down!");
            bodiesToChange.push_back(groundDownBody);
            log("UserDataA: %s ", groundDownBody);
            /*b2Vec2 force(0.0f, -10.0f);
            groundDownBody->ApplyForceToCenter(force, true);*/
            /* groundDownBody->SetType(b2_dynamicBody);
             if (!world->IsLocked()) {
                 log("set xong");
             }*/
        }
    }
    else {
        log("khong cham ground down");
    }

    if ((bodyA->GetType() == b2_dynamicBody && bodyB->GetType() == b2_staticBody) ||
        (bodyB->GetType() == b2_dynamicBody && bodyA->GetType() == b2_staticBody)) {
        isCharacterOnGround = true;
    }
}



void Scene2::update(float delta)
{
    world->Step(delta, 8, 3);

    if (!world->IsLocked()) {
        for (b2Body* body : bodiesToChange) {
            if (bodiesToChange.size() > 1) {
                body->SetType(b2_dynamicBody);

                // Áp dụng lực lớn hơn để nó rơi nhanh hơn
                b2Vec2 force(0.0f, -1000.0f);
                body->ApplyForceToCenter(force, true);
                log("Body ground_down đã chuyển thành dynamic");
            }
        }
    }

    //for (auto body : bodiesToChange) {
    //    if (body->GetUserData() != nullptr && strcmp(static_cast<const char*>(body->GetUserData()), "ground_down") == 0) {
    //        b2Vec2 position = body->GetPosition();
    //        platformSprite->setPosition(position.x * Common::PIXELS_PER_METER,
    //            position.y * Common::PIXELS_PER_METER);
    //        break; // Chỉ cần cập nhật một lần cho platformSprite
    //    }
    //}

    // Cập nhật vị trí cho nhân vật
    for (auto child : this->getChildren()) {
        auto characterSprite = dynamic_cast<Sprite*>(child);
        if (characterSprite && characterSprite->getUserData()) {
            b2Body* charBody = static_cast<b2Body*>(characterSprite->getUserData());
            b2Vec2 position = charBody->GetPosition();
            characterSprite->setPosition(position.x * Common::PIXELS_PER_METER,
                position.y * Common::PIXELS_PER_METER);

            if (isCharacterOnGround) {
                b2Vec2 velocity = charBody->GetLinearVelocity();
                velocity.x = 0.0f;
                charBody->SetLinearVelocity(velocity);
            }
        }
    }

}

//void Scene2::BeginContact(b2Contact* contact)
//{
//    b2Fixture* fixtureA = contact->GetFixtureA();
//    b2Fixture* fixtureB = contact->GetFixtureB();
//
//    // Kiểm tra xem nhân vật có va chạm với mặt đất không
//    b2Body* bodyA = fixtureA->GetBody();
//    b2Body* bodyB = fixtureB->GetBody();
//
//    // Giả sử body của nhân vật là body di động duy nhất
//    if ((bodyA->GetType() == b2_dynamicBody && bodyB->GetType() == b2_staticBody) ||
//        (bodyB->GetType() == b2_dynamicBody && bodyA->GetType() == b2_staticBody)) {
//        isCharacterOnGround = true; // Nhân vật đã tiếp đất
//    }
//}

cocos2d::Animation* Scene2::createAnimation(std::string prefixName, int pFramesOrder, float delay) {
    Vector<SpriteFrame*> animFrames;
    for (int i = 1; i <= pFramesOrder; i++) {
        char buffer[20] = { 0 };
        sprintf(buffer, "%d.png", i);
        std::string str = prefixName + buffer;
        auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(str);
        if (frame) {
            animFrames.pushBack(frame);
        }
        else {
            CCLOG("Frame not found: %s", str.c_str());
        }
    }
    return Animation::createWithSpriteFrames(animFrames, delay);
}

void Scene2::menuCloseCallback(Ref* pSender)
{
    Director::getInstance()->end();
}
