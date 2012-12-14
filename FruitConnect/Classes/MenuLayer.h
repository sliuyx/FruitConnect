/*
 * MenuLayer.h
 *
 *  Created on: Sep 17, 2012
 *      Author: liuyaxin
 */

#ifndef MENULAYER_H_
#define MENULAYER_H_

#include "cocos2d.h"

class MenuLayer : public cocos2d::CCLayer
{
public:
    virtual bool init();

    static cocos2d::CCScene* scene();
    
    virtual void keyBackClicked();

    void classicGameCallback(CCObject* pSender);
    void endlessGameCallback(CCObject* pSender);
    void mushroomGameCallback(CCObject* pSender);
    void shopCallback(CCObject* pSender);
    void moreCallback(CCObject* pSender);
    void threeCallback(CCObject* pSender);
    void sixCallback(CCObject* pSender);
    void cancelCallback(CCObject* pSender);
    
    void fruit1Callback(CCNode* pSender);
    void fruit2Callback(CCNode* pSender);
    
    CCLayer* m_menuLayer;
    CCLayer* m_endless_menu_layer;

  CREATE_FUNC(MenuLayer);
};

#endif /* MENULAYER_H_ */
