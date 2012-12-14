/*
 * MenuLayer.cpp
 *
 *  Created on: Sep 17, 2012
 *      Author: liuyaxin
 */

#include "MenuLayer.h"
#include "SimpleAudioEngine.h"
#include "LevelLayer.h"
#include "Common.h"
#include "GameLayer.h"
#include "ShopLayer.h"
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include <jni.h>
#include "platform/android/jni/JniHelper.h"
#endif

using namespace cocos2d;
using namespace CocosDenshion;

CCScene* MenuLayer::scene()
{
  // 'scene' is an autorelease object
  CCScene *scene = CCScene::create();

  // 'layer' is an autorelease object
  MenuLayer *layer = MenuLayer::create();
    layer->setTag(LAYER_MENU);
  // add layer as a child to scene
  scene->addChild(layer);
    scene->setTag(SCENE_MENU);

  // return the scene
  return scene;
}

// on "init" you need to initialize your instance
bool MenuLayer::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !CCLayer::init() )
    {
        return false;
    }
    this->setKeypadEnabled(true);

    CCSize size = CCDirector::sharedDirector()->getWinSize();

    CCSprite* pBackground = CCSprite::create("background.png");
    pBackground->setPosition( ccp(size.width/2, size.height/2));
    this->addChild(pBackground);
    
    CCSprite* pLeftTree = CCSprite::create("left_tree.png");
    pLeftTree->setPosition(ccp(137, 680));
    this->addChild(pLeftTree);
    
    CCSprite* pRightTree = CCSprite::create("right_tree.png");
    pRightTree->setPosition(ccp(460, 810));
    this->addChild(pRightTree);
    
    CCSprite* pMenuBox = CCSprite::create("menu_box.png");
    pMenuBox->setPosition(ccp(size.width/2, -105));
    this->addChild(pMenuBox);
    
    pMenuBox->runAction(CCMoveTo::create(0.5f, ccp(size.width/2, 395)));

    CCSprite* pLogo = CCSprite::create("logo.png");
    pLogo->setPosition(ccp(size.width/2, size.height/2+290));
    this->addChild(pLogo);
    pLogo->runAction(CCRepeatForever::create(CCSequence::createWithTwoActions(CCMoveTo::create(2.0, ccp(size.width/2, size.height/2+310)),
                                                                                CCMoveTo::create(2.0, ccp(size.width/2, size.height/2+290)))));

    CCMenuItemImage *pClassicItem = CCMenuItemImage::create(
                                            "menu_classic.png",
                                            "menu_classic_pressed.png",
                                            this,
                                            menu_selector(MenuLayer::classicGameCallback));
    pClassicItem->setPosition(ccp(size.width/2, 520-500));
    
    
    CCMenuItemImage *pMushroomItem = CCMenuItemImage::create("menu_mushroom.png", "menu_mushroom_pressed.png",
                                                             this, menu_selector(MenuLayer::mushroomGameCallback));
    pMushroomItem->setPosition(ccp(size.width/2, 420-500));
    
    CCMenuItemImage *pEndlessItem = CCMenuItemImage::create(
                                            "menu_endless.png",
                                            "menu_endless_pressed.png",
                                            this, menu_selector(MenuLayer::endlessGameCallback));
    pEndlessItem->setPosition(ccp(size.width/2, 320-500));
    
    
    
    CCMenuItemImage *pShopItem = CCMenuItemImage::create("menu_shop.png", "menu_shop.png",
                                                         this, menu_selector(MenuLayer::shopCallback));
    pShopItem->setPosition(ccp(180,90));
    
    CCMenuItemImage *pMoreItem = CCMenuItemImage::create("menu_more.png", "menu_more.png",
                                                           this, menu_selector(MenuLayer::moreCallback));
    pMoreItem->setPosition(ccp(460,90));

    m_menuLayer = CCMenu::create(pClassicItem, pEndlessItem, pMushroomItem, NULL);
    m_menuLayer->setPosition(CCPointZero);
    this->addChild(m_menuLayer);
    
    CCMenuItemImage *pThreeItem = CCMenuItemImage::create("menu_three_minutes.png", "menu_three_minutes_pressed.png",
                                                          this, menu_selector(MenuLayer::threeCallback));
    pThreeItem->setPosition(ccp(size.width/2, 520-500));
    CCMenuItemImage *pSixItem = CCMenuItemImage::create("menu_six_minutes.png", "menu_six_minutes_pressed.png",
                                                        this, menu_selector(MenuLayer::sixCallback));
    pSixItem->setPosition(ccp(size.width/2, 420-500));
    CCMenuItemImage *pCancelItem = CCMenuItemImage::create("menu_cancel.png", "menu_cancel_pressed.png",
                                                           this, menu_selector(MenuLayer::cancelCallback));
    pCancelItem->setPosition(ccp(size.width/2, 320-500));
    m_endless_menu_layer = CCMenu::create(pThreeItem, pSixItem, pCancelItem, NULL);
    m_endless_menu_layer->setPosition(CCPointZero);
    this->addChild(m_endless_menu_layer);
    
    pClassicItem->runAction(CCMoveTo::create(0.5f, ccp(size.width/2, 520)));
    pMushroomItem->runAction(CCMoveTo::create(0.5f, ccp(size.width/2, 420)));
    pEndlessItem->runAction(CCMoveTo::create(0.5f, ccp(size.width/2, 320)));

    CCSprite* pMenuGrass = CCSprite::create("menu_grass.png");
    pMenuGrass->setPosition(ccp(size.width/2, 150));
    this->addChild(pMenuGrass);
    
    CCMenu* pMenuBottom = CCMenu::create(pShopItem, pMoreItem, NULL);
    pMenuBottom->setPosition(CCPointZero);
    this->addChild(pMenuBottom);
    
    CCSprite* pFruit1 = CCSprite::create("menu_fruit1.png");
    pFruit1->setPosition(ccp(96, 513));
    pFruit1->setScale(0.1);
    this->addChild(pFruit1);
    pFruit1->runAction(CCSequence::create(CCScaleTo::create(0.3f, 1.0f), CCCallFuncN::create(pFruit1, callfuncN_selector(MenuLayer::fruit1Callback)), NULL));
    
    CCSprite* pFruit2 = CCSprite::create("menu_fruit2.png");
    pFruit2->setPosition(ccp(56, 379));
    pFruit2->setScale(0.1);
    this->addChild(pFruit2);
    pFruit2->runAction(CCSequence::create(CCScaleTo::create(0.6f, 1.0f), CCCallFuncN::create(pFruit2, callfuncN_selector(MenuLayer::fruit2Callback)), NULL));
    
    CCSprite* pFruit3 = CCSprite::create("menu_fruit3.png");
    pFruit3->setPosition(ccp(102, 281));
    pFruit3->setScale(0.1);
    this->addChild(pFruit3);
    pFruit3->runAction(CCSequence::create(CCScaleTo::create(0.4f, 1.0f), CCCallFuncN::create(pFruit3, callfuncN_selector(MenuLayer::fruit1Callback)), NULL));
    
    CCSprite* pFruit4 = CCSprite::create("menu_fruit4.png");
    pFruit4->setPosition(ccp(584, 503));
    pFruit4->setScale(0.1);
    this->addChild(pFruit4);
    pFruit4->runAction(CCSequence::create(CCScaleTo::create(0.2f, 1.0f), CCCallFuncN::create(pFruit4, callfuncN_selector(MenuLayer::fruit2Callback)), NULL));
    
    CCSprite* pFruit5 = CCSprite::create("menu_fruit5.png");
    pFruit5->setPosition(ccp(559, 384));
    pFruit5->setScale(0.1);
    this->addChild(pFruit5);
    pFruit5->runAction(CCSequence::create(CCScaleTo::create(0.5f, 1.0f), CCCallFuncN::create(pFruit5, callfuncN_selector(MenuLayer::fruit1Callback)), NULL));
    
    return true;
}

void MenuLayer::classicGameCallback(CCObject* pSender)
{
  CCScene* level = LevelLayer::scene(CLASSIC_MODE);
  CCTransitionFade* transition = CCTransitionFade::create(0.6, level);
  CCDirector::sharedDirector()->pushScene(transition);
}

void MenuLayer::endlessGameCallback(CCObject* pSender)
{
    m_menuLayer->runAction(CCMoveTo::create(0.5, ccp(0,-500)));
    m_endless_menu_layer->runAction(CCSequence::create(CCDelayTime::create(0.5), CCMoveTo::create(0.5, ccp(0, 500)), NULL));
}

void MenuLayer::threeCallback(cocos2d::CCObject *pSender) {
    CCTransitionFade* transition = CCTransitionFade::create(0.6, GameLayer::scene(ENDLESS_MODE, 0));
    CCDirector::sharedDirector()->pushScene(transition);  
}

void MenuLayer::sixCallback(cocos2d::CCObject *pSender) {
    CCTransitionFade* transition = CCTransitionFade::create(0.6, GameLayer::scene(ENDLESS_MODE, 1));
    CCDirector::sharedDirector()->pushScene(transition);  
}

void MenuLayer::cancelCallback(cocos2d::CCObject *pSender) {
    m_endless_menu_layer->runAction(CCMoveTo::create(0.5, CCPointZero));
    m_menuLayer->runAction(CCSequence::create(CCDelayTime::create(0.5), CCMoveTo::create(0.5f, CCPointZero), NULL));
}

void MenuLayer::mushroomGameCallback(cocos2d::CCObject *pSender)
{
    CCScene* level = LevelLayer::scene(MUSHROOM_MODE);
    CCTransitionFade* transition = CCTransitionFade::create(0.6, level);
    CCDirector::sharedDirector()->pushScene(transition);
}

void MenuLayer::shopCallback(cocos2d::CCObject *pSender)
{
    CCTransitionFade* transition = CCTransitionFade::create(0.6, ShopLayer::scene());
    CCDirector::sharedDirector()->pushScene(transition);
}

void MenuLayer::moreCallback(cocos2d::CCObject *pSender)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    JniMethodInfo mInfo;
    bool isHave = JniHelper::getStaticMethodInfo(mInfo, "com/xxstudio/llk/Cocos2dHelper", "showMore", "()V");
    if (isHave) {
        mInfo.env->CallStaticVoidMethod(mInfo.classID, mInfo.methodID);
    }
#endif
}

void MenuLayer::fruit1Callback(cocos2d::CCNode *pSender)
{
    CCPoint pt = pSender->getPosition();
    pSender->runAction(CCRepeatForever::create(CCSequence::createWithTwoActions(
            CCMoveTo::create(2.0, ccp(pt.x+abs(rand())%10+5, pt.y+abs(rand())%10+5)),
                             CCMoveTo::create(2.0, ccp(pt.x, pt.y)))));
}

void MenuLayer::fruit2Callback(cocos2d::CCNode *pSender)
{
    CCPoint pt = pSender->getPosition();
    pSender->runAction(CCRepeatForever::create(CCSequence::createWithTwoActions(
            CCMoveTo::create(2.0, ccp(pt.x-abs(rand())%10-5, pt.y-abs(rand())%10-5)), CCMoveTo::create(2.0, ccp(pt.x, pt.y)))));
}

void MenuLayer::keyBackClicked() {
    CCDirector::sharedDirector()->end();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}



