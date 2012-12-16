//
//  ShopLayer.cpp
//  FruitConnect
//
//  Created by 柳亚鑫 on 12-12-4.
//  Copyright 2012年 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include "ShopLayer.h"
#include "MenuLayer.h"
#include "CommonFunctions.h"
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include <jni.h>
#include "platform/android/jni/JniHelper.h"
#endif


using namespace cocos2d;

CCScene* ShopLayer::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    
    // 'layer' is an autorelease object
    ShopLayer *layer = ShopLayer::create();
    layer->setTag(LAYER_SHOP);
    // add layer as a child to scene
    scene->addChild(layer);
    scene->setTag(SCENE_SHOP);
    
    // return the scene
    return scene;
}

bool ShopLayer::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !CCLayer::init() )
    {
        return false;
    }
    this->setKeypadEnabled(true);
    
    initBackground(CCDirector::sharedDirector()->getWinSize());
    initScrollArea();
    initOwnedItems();
    initScrollbar();
    
    return true;
}

void ShopLayer::keyBackClicked() {
    homeCallback(NULL);
}

void ShopLayer::keyMenuClicked() {
    callbackMenu();
}

void ShopLayer::homeCallback(cocos2d::CCObject *pSender) {
    CCTransitionFade* transition = CCTransitionFade::create(0.3, MenuLayer::scene());
    CCDirector::sharedDirector()->pushScene(transition);
}

void ShopLayer::onEnter() {
    CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, 0, true);
    CCLayer::onEnter();
}

void ShopLayer::onExit() {
    CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
    CCLayer::onExit();
}

void ShopLayer::drawNumbersToLayer(cocos2d::CCLayer *layer, int number, cocos2d::CCPoint point, bool left, float scale) {
    drawNumbersToLayer("shop_numbers.png", layer, number, point, left, scale);
}

void ShopLayer::drawNumbersToLayer(const char* image, cocos2d::CCLayer *layer, int number, CCPoint point, bool left, float scale) {
    int length = getNumberLength(number);
    int l = 25*scale;
    int width = (length-1)*l;
    if (left) {
        point.x = point.x+width;
    }
    do {
        int n = number%10;
        CCSprite* noSprite = CCSprite::create(image, CCRect(25*n, 0, 25, 30));
        noSprite->setPosition(point);
        noSprite->setScale(scale);
        layer->addChild(noSprite);
        point.x-=l;
        number = number/10;
    } while (number > 0);
}

void ShopLayer::initBackground(CCSize size) {
    CCSprite* pBackground = CCSprite::create("shop_bg.png");
    pBackground->setPosition(ccp(size.width/2, size.height/2));
    this->addChild(pBackground);
}

void ShopLayer::initOwnedItems() {
    m_owned_items = CCLayer::create();
    this->addChild(m_owned_items);
    CCSprite* gold = CCSprite::create("gold_coin.png");
    gold->setScale(0.85);
    gold->setPosition(ccp(100, 850));
    this->addChild(gold);
    CCSprite* tip = CCSprite::create("tip_item.png");
    tip->setScale(0.5);
    tip->setPosition(ccp(250,850));
    this->addChild(tip);
    CCSprite* rerange = CCSprite::create("rerange_item.png");
    rerange->setScale(0.5);
    rerange->setPosition(ccp(365, 850));
    this->addChild(rerange);
    CCSprite* time = CCSprite::create("time_item.png");
    time->setScale(0.5);
    time->setPosition(ccp(480, 850));
    this->addChild(time);
    CCSprite* seprator = CCSprite::create("shop_top_seperator.png");
    seprator->setPosition(ccp(320, 810));
    this->addChild(seprator);
    refreshOwnedItems();
}

void ShopLayer::refreshOwnedItems() {
    m_owned_items->removeAllChildrenWithCleanup(true);
    m_gold_number = CCLayer::create();
    m_gold_number_red = CCLayer::create();
    m_owned_items->addChild(m_gold_number);
    m_owned_items->addChild(m_gold_number_red);
    m_gold_number_red->setVisible(false);
    int gold_cnt = getGoldCount();
    drawNumbersToLayer(m_gold_number, gold_cnt, ccp(135,850), true, 0.8);
    drawNumbersToLayer("shop_numbers_red.png", m_gold_number_red, gold_cnt, ccp(135,850), true, 0.8);
    int tip_cnt = getTipItemCount();
    drawNumbersToLayer(m_owned_items, tip_cnt, ccp(285,850), true, 0.8);
    int rerange_cnt = getRerangeItemCount();
    drawNumbersToLayer(m_owned_items, rerange_cnt, ccp(400,850), true, 0.8);
    int time_cnt = getTimeItemCount();
    drawNumbersToLayer(m_owned_items, time_cnt, ccp(515, 850), true, 0.8);
}

void ShopLayer::initScrollArea() {
    m_scroll_area = CCLayerColor::create();
    m_scroll_area->setPosition(CCPointZero);
    m_scroll_area->setContentSize(CCSize(450, 740));
    this->addChild(m_scroll_area);
    drawItemLine(TIP, 10, 20, ccp(80,710));
    drawItemLine(RERANGE, 10, 30, ccp(80,610));
    drawItemLine(TIME, 10, 50, ccp(80,510));
    drawItemLine(MIXED, 5, 50, ccp(80,410));
    drawItemLine(TIP, 20, 32, ccp(80,310));
    drawItemLine(RERANGE, 20, 48, ccp(80,210));
    drawItemLine(TIME, 20, 80, ccp(80,110));
    drawItemLine(MIXED, 10, 80, ccp(80,10));
    drawItemLine(TIP, 50, 60, ccp(80,-90));
    drawItemLine(RERANGE, 50, 90, ccp(80,-190));
    drawItemLine(TIME, 50, 150, ccp(80,-290));
    drawItemLine(MIXED, 25, 150, ccp(80,-390));
    CCSprite* upCover = CCSprite::create("shop_up_cover.png");
    upCover->setPosition(ccp(0,960));
    upCover->setAnchorPoint(ccp(0,1));
    this->addChild(upCover);
    CCSprite* downCover = CCSprite::create("shop_down_cover.png");
    downCover->setPosition(ccp(0,0));
    downCover->setAnchorPoint(ccp(0,0));
    this->addChild(downCover);
    CCMenuItemImage *pHomeItem = CCMenuItemImage::create("button_home.png",
                                                         "button_home.png",
                                                         this,
                                                         menu_selector(ShopLayer::homeCallback));
    pHomeItem->setPosition(ccp(572, 68));
    CCMenuItemImage *pFree = CCMenuItemImage::create(getPngPath("get_free_coins.png")->getCString(),
                                                    getPngPath("get_free_coins_pressed.png")->getCString(),
                                                     this,
                                                     menu_selector(ShopLayer::freeCoinCallback));
    pFree->setPosition(ccp(280, 90));
    CCMenu* menu = CCMenu::create(pHomeItem, pFree, NULL);
    menu->setPosition(CCPointZero);
    this->addChild(menu);
    CCSprite* seperatorDown = CCSprite::create("shop_top_seperator.png");
    seperatorDown->setPosition(ccp(320, 150));
    this->addChild(seperatorDown);
}

void ShopLayer::initScrollbar() {
    m_scroll_bar = CCLayer::create();
    this->addChild(m_scroll_bar);
    CCSprite* line = CCSprite::create("shop_scroll_bg.png");
    line->setPosition(ccp(570, 540));
    this->addChild(line);
    refreshScrollbar();
}

void ShopLayer::refreshScrollbar() {
    m_scroll_bar->removeAllChildrenWithCleanup(true);
    int max = 100*12-660;
    int y = m_scroll_area->getPositionY();
    int current = 690-y*300/max;
    if (current > 690) {
        current = 690;
    } else if (current < 390) {
        current = 390;
    }
    
    CCSprite* block = CCSprite::create("shop_scroll_block.png");
    block->setPosition(ccp(570, current));
    m_scroll_bar->addChild(block);
}

void ShopLayer::drawItemLine(int type, int item_number, int gold_number, CCPoint point) {
    if (type == TIP) {
        CCSprite* icon = CCSprite::create("tip_item.png");
        icon->setScale(0.625);
        icon->setPosition(ccp(135, point.y+50));
        m_scroll_area->addChild(icon);
    } else if (type == RERANGE) {
        CCSprite* icon = CCSprite::create("rerange_item.png");
        icon->setScale(0.625);
        icon->setPosition(ccp(135, point.y+50));
        m_scroll_area->addChild(icon);
    } else if (type == TIME) {
        CCSprite* icon = CCSprite::create("time_item.png");
        icon->setScale(0.625);
        icon->setPosition(ccp(135, point.y+50));
        m_scroll_area->addChild(icon);
    } else if (type == MIXED) {
        CCSprite* icon = CCSprite::create("mixed_item.png");
        icon->setScale(0.625);
        icon->setPosition(ccp(135, point.y+50));
        m_scroll_area->addChild(icon);
    }
    if (item_number > 1) {
        CCSprite* multiply = CCSprite::create("shop_multiply.png");
        multiply->setPosition(ccp(177, point.y+50));
        m_scroll_area->addChild(multiply);
        drawNumbersToLayer(m_scroll_area, item_number, ccp(205, point.y+50), true, 1);
    }
    CCSprite* gold = CCSprite::create("gold_coin.png");
    gold->setPosition(ccp(290, point.y+50));
    m_scroll_area->addChild(gold);
    drawNumbersToLayer(m_scroll_area, gold_number, ccp(330, point.y+50), true, 1);
    BuyButtonSprite* buy = BuyButtonSprite::create(getPngPath("shop_buy.png")->getCString(), type, item_number, gold_number, this);
    buy->setPosition(ccp(470, point.y+50));
    m_scroll_area->addChild(buy);
    CCSprite* seperator = CCSprite::create("shop_line_seperator.png");
    seperator->setPosition(ccp(320, point.y));
    m_scroll_area->addChild(seperator);
}

void ShopLayer::freeCoinCallback(cocos2d::CCObject *pSender) {
    showOffers();
}

void ShopLayer::showOffers() {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    JniMethodInfo mInfo;
    bool isHave = JniHelper::getStaticMethodInfo(mInfo, "com/xxstudio/llk/Cocos2dHelper", "showOffers", "()V");
    if (isHave) {
        mInfo.env->CallStaticVoidMethod(mInfo.classID, mInfo.methodID);
    }
#endif
}

void ShopLayer::spendPoint(int itemType, int itemCount, int point) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    JniMethodInfo mInfo;
    bool isHave = JniHelper::getStaticMethodInfo(mInfo, "com/xxstudio/llk/Cocos2dHelper", "spendPoints", "(III)V");
    if (isHave) {
        mInfo.env->CallStaticVoidMethod(mInfo.classID, mInfo.methodID, itemType, itemCount, point);
    }
#endif
}

bool ShopLayer::ccTouchBegan(CCTouch *touch, CCEvent *event) {
    CCRect rect(85, 150, 450, 660);
    if (rect.containsPoint(touch->getLocation())) {
        m_beginPos = touch->getLocation();
        m_lastPos = m_beginPos;
        return true;
    }
    return false;
}

void ShopLayer::ccTouchMoved(cocos2d::CCTouch *touch, cocos2d::CCEvent *event) {
    CCPoint touchLoc = touch->getLocation();
    float moveY = touchLoc.y - m_lastPos.y;
    
    CCPoint curPos = m_scroll_area->getPosition();
    CCPoint nextPos = ccp(curPos.x, curPos.y+moveY);
    m_scroll_area->setPosition(nextPos);
    m_lastPos = touchLoc;
    refreshScrollbar();
}

void ShopLayer::ccTouchEnded(cocos2d::CCTouch *touch, cocos2d::CCEvent *event) {
    int minY = 0, maxY = 100*12-660;
    int pointY = (int)(m_scroll_area->getPositionY());
    if (pointY > maxY) {
        m_scroll_area->runAction(CCMoveTo::create(0.2f, ccp(m_scroll_area->getPositionX(), maxY)));
        return;
    }
    if (pointY < minY) {
        m_scroll_area->runAction(CCMoveTo::create(0.2f, ccp(m_scroll_area->getPositionX(), minY)));
        return;
    }
    refreshScrollbar();
}

void ShopLayer::ccTouchBegan(BuyButtonSprite *sprite, cocos2d::CCTouch *touch, cocos2d::CCEvent *event) {
    
}

void ShopLayer::ccTouchMoved(BuyButtonSprite *sprite, cocos2d::CCTouch *touch, cocos2d::CCEvent *event) {
    
}

void ShopLayer::ccTouchEnded(BuyButtonSprite *sprite, cocos2d::CCTouch *touch, cocos2d::CCEvent *event) {
    int gold = getGoldCount();
    if (sprite->getPrice() <= gold) {
        spendPoint(sprite->getType(), sprite->getCount(), sprite->getPrice());
//        saveGoldCount(gold-sprite->getPrice());
//        if (sprite->getType() == TIP) {
//            saveTipItemCount(getTipItemCount()+sprite->getCount());
//        } else if (sprite->getType() == RERANGE) {
//            saveRerangeItemCount(getRerangeItemCount()+sprite->getCount());
//        } else if (sprite->getType() == TIME) {
//            saveTimeItemCount(getTimeItemCount()+sprite->getCount());
//        } else if (sprite->getType() == MIXED) {
//            saveTipItemCount(getTipItemCount()+sprite->getCount());
//            saveRerangeItemCount(getRerangeItemCount()+sprite->getCount());
//            saveTimeItemCount(getTimeItemCount()+sprite->getCount());
//        }
//        FLUSH_DATA;
//        refreshOwnedItems();
    } else {
        m_gold_number->runAction(CCSequence::create(CCHide::create(), CCDelayTime::create(1.0f), CCShow::create(), NULL));
        
        m_gold_number_red->runAction(CCSequence::create(CCDelayTime::create(0.1f),
                                                        CCShow::create(), CCDelayTime::create(0.1f),
                                                        CCHide::create(), CCDelayTime::create(0.1f),
                                                        CCShow::create(), CCDelayTime::create(0.1f),
                                                        CCHide::create(), CCDelayTime::create(0.1f),
                                                        CCShow::create(), CCDelayTime::create(0.1f),
                                                        CCHide::create(), CCDelayTime::create(0.1f),
                                                        CCShow::create(), CCDelayTime::create(0.1f),
                                                        CCHide::create(), CCDelayTime::create(0.1f),
                                                        CCDelayTime::create(0.1f),
                                                        NULL));
    }
}

BuyButtonSprite::BuyButtonSprite(int type, int count, int price, BuyButtonTouchListener* listener)
    : m_type(type), m_count(count), m_price(price), m_listener(listener) {
}

BuyButtonSprite* BuyButtonSprite::create(const char *file, int type, int count, int price, BuyButtonTouchListener *listener) {
    BuyButtonSprite *sprite = new BuyButtonSprite(type, count, price, listener);
    
    if (sprite && sprite->initWithFile(file)) {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return NULL;
}

bool BuyButtonSprite::ccTouchBegan(cocos2d::CCTouch *touch, cocos2d::CCEvent *event) {
    bool bRet = false;
    if (containsTouchLocation(touch) && isVisible()) {
        origPos = getPosition();
        if (m_listener != NULL) {
            m_listener->ccTouchBegan(this, touch, event);
        }
        setPosition(ccp(origPos.x, origPos.y-5));
        bRet = true;
    }
    return bRet;
}

void BuyButtonSprite::ccTouchMoved(cocos2d::CCTouch *touch, cocos2d::CCEvent *event) {
    if (m_listener != NULL) {
        m_listener->ccTouchMoved(this, touch, event);
    }
}

void BuyButtonSprite::ccTouchEnded(cocos2d::CCTouch *touch, cocos2d::CCEvent *event) {
    if (containsTouchLocation(touch)) {
        if (m_listener != NULL) {
            m_listener->ccTouchEnded(this, touch, event);
        }
    }
    setPosition(origPos);
}


