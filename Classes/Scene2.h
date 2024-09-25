#ifndef __SCENE2_H__
#define __SCENE2_H__

#include "cocos2d.h"
#include "Box2D/Box2D.h"

class Scene2 : public cocos2d::Scene, public b2ContactListener
{
public:
    // Hàm tạo scene
    static cocos2d::Scene* createScene();

    // Hàm khởi tạo
    virtual bool init() override;

    // Callback cho nút đóng
    void menuCloseCallback(cocos2d::Ref* pSender);
    void update(float delta);

    // Xử lý sự kiện va chạm
    void BeginContact(b2Contact* contact) override;

    void onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
    void onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);

    cocos2d::Animation* createAnimation(std::string prefixName, int pFramesOrder, float delay);
    cocos2d::Animation* animation;

    // Macro để tạo lớp
    CREATE_FUNC(Scene2);

private:
    void setPhysicworld(cocos2d::PhysicsWorld* world) { world = world; }
    //b2World* world; // Thế giới vật lý
    //bool isCharacterOnGround;
    /*void setPhysicworld(cocos2d::PhysicsWorld* world) { world = world; }*/
    b2World* world;
    bool isCharacterOnGround; 
    b2Body* charBody;
    void moveCharacter(float x, float y);
};

#endif // __SCENE2_H__
