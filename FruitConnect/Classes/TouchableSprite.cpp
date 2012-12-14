/*
 * TouchableSprite.cpp
 *
 *  Created on: Oct 6, 2012
 *      Author: liuyaxin
 */

#include "TouchableSprite.h"

TouchableSprite::TouchableSprite() {
}

TouchableSprite::~TouchableSprite() {
}

TouchableSprite* TouchableSprite::touchSpriteWithFile(const char *file) {
    TouchableSprite *sprite = new TouchableSprite();

    if (sprite && sprite->initWithFile(file)) {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return NULL;
}

bool TouchableSprite::initWithFile(const char *file) {
    return CCSprite::initWithFile(file);
}

void TouchableSprite::onEnter() {
    CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, 0, true);
    CCSprite::onEnter();
}

void TouchableSprite::onExit() {
    CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
    CCSprite::onExit();
}

CCRect TouchableSprite::rect() {
    CCSize size = getTexture()->getContentSize();
    return CCRectMake(-size.width / 2, -size.height / 2, size.width, size.height);
}

bool TouchableSprite::containsTouchLocation(cocos2d::CCTouch *touch) {
    return CCRect::CCRectContainsPoint(rect(), convertTouchToNodeSpaceAR(touch));}

bool TouchableSprite::ccTouchBegan(cocos2d::CCTouch *touch, cocos2d::CCEvent *event) {
    bool bRet = false;
    if (containsTouchLocation(touch) && isVisible()) {
        CCLog("Touchable Sprite Began");
        bRet = true;
    }
    return bRet;
}

void TouchableSprite::ccTouchMoved(cocos2d::CCTouch *touch, cocos2d::CCEvent *event) {
    CCLog("Touchable Sprite Moved");
}

void TouchableSprite::ccTouchEnded(cocos2d::CCTouch *touch, cocos2d::CCEvent *event) {
    CCLog("Touchable Sprite Ended");
}
