/*
 * LevelLayer.h
 *
 *  Created on: Sep 17, 2012
 *      Author: liuyaxin
 */

#ifndef LEVELLAYER_H_
#define LEVELLAYER_H_
#include "cocos2d.h"
#include "TouchableSprite.h"

class LevelSprite;

class LevelTouchListener {
public:
    virtual void onTouchEnded(CCTouch* touch, CCEvent* event) = 0;
    virtual void moveContainer(float x) = 0;
    virtual void onClick(LevelSprite* sprite) = 0;
};

class LevelLayer : public cocos2d::CCLayer, public LevelTouchListener {
public:
    virtual bool init();
  virtual bool init(int mode);

  static cocos2d::CCScene* scene(int mode);
    static LevelLayer* create(int mode);

  void backGameCallback(CCObject* pSender);
    virtual void onEnter();
    virtual void onExit();

    virtual bool ccTouchBegan(CCTouch *touch, CCEvent *event);
    virtual void ccTouchMoved(CCTouch *touch, CCEvent *event);
    virtual void ccTouchEnded(CCTouch *touch, CCEvent *event);
    
    virtual void onTouchEnded(CCTouch* touch, CCEvent* event);
    virtual void moveContainer(float x);
    virtual void onClick(LevelSprite* sprite);
    virtual void keyBackClicked();
    void preLoadEffect(CCTime dt);
    
private:
    int getEarnedStarCount();
    int getAllStarCount();
    int getPageCount();
    void refreshDotArea();
    int getLevelNo();
    int getPageLevelNo();
    int getPageColumnNo();
    int getHishcore(int level);
    int getLevelStarCount(int level, int levelScore);

    int m_currentPage;
    int m_mode;
    CCPoint m_beginPos;
    CCLayer* m_levels;
    CCLayer* m_dots;
public:
  CREATE_FUNC(LevelLayer);
};

class LevelSprite : public TouchableSprite {
public:
  LevelSprite(int level, LevelTouchListener *listener);

  static LevelSprite* create(const char* file, int level, LevelTouchListener* listener);

  int getLevel() { return m_level; }

  virtual bool ccTouchBegan(CCTouch *touch, CCEvent *event);
  virtual void ccTouchMoved(CCTouch *touch, CCEvent *event);
  virtual void ccTouchEnded(CCTouch *touch, CCEvent *event);
private:
  int m_level;
    CCPoint beginPos;
    CCPoint curPos;
  LevelTouchListener* m_listener;

};

#endif /* LEVELLAYER_H_ */
