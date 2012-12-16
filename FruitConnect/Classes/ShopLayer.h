//
//  ShopLayer.h
//  FruitConnect
//
//  Created by 柳亚鑫 on 12-12-4.
//  Copyright 2012年 __MyCompanyName__. All rights reserved.
//

#ifndef FruitConnect_ShopLayer_h
#define FruitConnect_ShopLayer_h

#include "cocos2d.h"
#include "TouchableSprite.h"

USING_NS_CC;

enum BuyButtonType {
    TIP, RERANGE, TIME, MIXED, GOLD
};

class BuyButtonSprite;
class BuyButtonTouchListener {
public:
    virtual void ccTouchBegan(BuyButtonSprite* sprite, CCTouch *touch, CCEvent *event) = 0;
    virtual void ccTouchMoved(BuyButtonSprite* sprite, CCTouch *touch, CCEvent *event) = 0;
    virtual void ccTouchEnded(BuyButtonSprite* sprite, CCTouch *touch, CCEvent *event) = 0;
};

class ShopLayer : public cocos2d::CCLayer, public BuyButtonTouchListener
{
public:
    virtual bool init();
    
    static cocos2d::CCScene* scene();
    
    virtual void keyBackClicked();
    virtual void keyMenuClicked();
    
    void homeCallback(CCObject* pSender);
    virtual void onEnter();
    virtual void onExit();
    
    void drawNumbersToLayer(CCLayer* layer, int number, CCPoint point, bool left, float scale);
    void drawNumbersToLayer(const char* image, CCLayer* layer, int number, CCPoint point, bool left, float scale);
    void initBackground(CCSize size);
    void initOwnedItems();
    void refreshOwnedItems();
    void initScrollArea();
    void initScrollbar();
    void refreshScrollbar();
    void drawItemLine(int type, int item_number, int gold_number, CCPoint point);
    void freeCoinCallback(CCObject* pSender);
    void showOffers();
    void spendPoint(int itemType, int itemCount, int point);
    
    virtual bool ccTouchBegan(CCTouch *touch, CCEvent *event);
    virtual void ccTouchMoved(CCTouch *touch, CCEvent *event);
    virtual void ccTouchEnded(CCTouch *touch, CCEvent *event);
    
    virtual void ccTouchBegan(BuyButtonSprite* sprite, CCTouch *touch, CCEvent *event);
    virtual void ccTouchMoved(BuyButtonSprite* sprite, CCTouch *touch, CCEvent *event);
    virtual void ccTouchEnded(BuyButtonSprite* sprite, CCTouch *touch, CCEvent *event);
    
    CREATE_FUNC(ShopLayer);
private:
    CCPoint m_beginPos;
    CCPoint m_lastPos;
    CCLayer* m_owned_items;
    CCLayer* m_gold_number;
    CCLayer* m_gold_number_red;
    CCLayerColor* m_scroll_area;
    CCLayer* m_scroll_bar;
};

class BuyButtonSprite : public TouchableSprite {
public:
    BuyButtonSprite(int type, int count, int price, BuyButtonTouchListener* listener);
    static BuyButtonSprite* create(const char* file, int type, int count, int price, BuyButtonTouchListener* listener);
    virtual bool ccTouchBegan(CCTouch *touch, CCEvent *event);
    virtual void ccTouchMoved(CCTouch *touch, CCEvent *event);
    virtual void ccTouchEnded(CCTouch *touch, CCEvent *event);
    int getType() { return m_type; }
    int getCount() { return m_count; }
    int getPrice() { return m_price; }
private:
    int m_type;
    int m_count;
    int m_price;
    CCPoint origPos;
    BuyButtonTouchListener* m_listener;
};

#endif
