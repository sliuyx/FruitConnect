/*
 * LevelLayer.cpp
 *
 *  Created on: Sep 17, 2012
 *      Author: liuyaxin
 */

#include "LevelLayer.h"
#include "SimpleAudioEngine.h"
#include "MenuLayer.h"
#include "GameLayer.h"
#include "Common.h"
#include "CommonFunctions.h"
#include "ClassicLevelData.h"
#include "MushroomLevelData.h"

using namespace cocos2d;
using namespace CocosDenshion;

CCScene* LevelLayer::scene(int mode)
{
  // 'scene' is an autorelease object
  CCScene *scene = CCScene::create();

  // 'layer' is an autorelease object
  LevelLayer *layer = LevelLayer::create(mode);
    layer->setTag(LAYER_LEVEL);
  // add layer as a child to scene
  scene->addChild(layer);
    scene->setTag(SCENE_LEVEL);

  // return the scene
  return scene;
}

void LevelLayer::onEnter() {
    CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, 0, true);
    CCLayer::onEnter();
}

void LevelLayer::onExit() {
    CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
    if (m_mode == CLASSIC_MODE) {
        SAVE_INTEGER("level_curpage_classic", m_currentPage);
    } else if (m_mode == MUSHROOM_MODE) {
        SAVE_INTEGER("level_curpage_mushroom", m_currentPage);
    }
    FLUSH_DATA;
    CCLayer::onExit();
}

LevelLayer* LevelLayer::create(int mode)
{
    LevelLayer *pRet = new LevelLayer();
    if (pRet && pRet->init(mode))
    {
        pRet->autorelease();
        return pRet;
    }
    else
    {
        delete pRet;
        pRet = NULL;
        return NULL;
    }
}

bool LevelLayer::init() {
    return init(CLASSIC_MODE);
}

// on "init" you need to initialize your instance
bool LevelLayer::init(int mode)
{
    //////////////////////////////
    // 1. super init first
    if ( !CCLayer::init() )
    {
        return false;
    }
    this->setKeypadEnabled(true);
    m_mode = mode;
    if (m_mode == CLASSIC_MODE) {
        m_currentPage = LOAD_INTEGER("level_curpage_classic", 0);
    } else if (m_mode == MUSHROOM_MODE) {
        m_currentPage = LOAD_INTEGER("level_curpage_mushroom", 0);
    }

    CCSize size = CCDirector::sharedDirector()->getWinSize();

    CCSprite* pBackground = CCSprite::create("background.png");
    pBackground->setPosition(ccp(size.width/2, size.height/2));
    this->addChild(pBackground);
    
    CCSprite* pLeftTree = CCSprite::create("left_tree.png");
    pLeftTree->setPosition(ccp(137, 680));
    this->addChild(pLeftTree);
    
    CCSprite* pLevelBox = CCSprite::create("level_box.png");
    pLevelBox->setPosition(ccp(size.width/2, 468));
    this->addChild(pLevelBox);
    
    CCSprite* pTitle = NULL;
    if (m_mode == CLASSIC_MODE) {
        pTitle = CCSprite::create("level_title_classic.png");
    } else if (m_mode == MUSHROOM_MODE) {
        pTitle = CCSprite::create("level_title_mushroom.png");
    }
    if (pTitle != NULL) {
        pTitle->setPosition(ccp(size.width/2, 870));
        this->addChild(pTitle);
    }
    
    
    CCSprite* pBoxFooter = CCSprite::create("box_footer.png");
    pBoxFooter->setPosition(ccp(size.width/2, 40));
    this->addChild(pBoxFooter);

    CCMenuItemImage *pReturnItem = CCMenuItemImage::create(
                                            "level_home_button.png",
                                            "level_home_button.png",
                                            this,
                                            menu_selector(LevelLayer::backGameCallback) );
    pReturnItem->setPosition(ccp(30, 928));

    CCMenu* pMenu = CCMenu::create(pReturnItem, NULL);
    pMenu->setPosition(CCPointZero);
    this->addChild(pMenu);

    m_levels = CCLayer::create();
    m_levels->setPosition(CCPointZero);
    
    int initX = size.width/2 - 200, initY = size.height/2 + 210, stepX = 100, stepY = 115;
    int lastScore = 100;
    int levelNo = getLevelNo();
    int pageLevelNo = getPageLevelNo();
    int columnNo = getPageColumnNo();
    int earnedStarCount = 0;
    for (int i = 0; i < levelNo; i++) {
        int page = i / pageLevelNo;
        int x = (i%pageLevelNo) % columnNo;
        int y = (i%pageLevelNo) / columnNo;

        int levelX = initX + x * stepX + page*640;
        int levelY = initY - y * stepY;
        int levelScore = getHishcore(i);
        if (levelScore > 0 || lastScore > 0) {
          LevelSprite* pLevel = LevelSprite::create("level_back.png", i, this);
          pLevel->setPosition(ccp(levelX, levelY));
            m_levels->addChild(pLevel);
            int starCount = getLevelStarCount(i, levelScore);
          if (starCount > 0) {
              CCSprite* pStar1 = CCSprite::create("level_star.png");
              pStar1->setPosition(ccp(levelX-20, levelY-37));
              m_levels->addChild(pStar1);
          }
          if (starCount > 1) {
              CCSprite* pStar2 = CCSprite::create("level_star.png");
              pStar2->setPosition(ccp(levelX, levelY-37));
              m_levels->addChild(pStar2);
          }
          if (starCount > 2){
              CCSprite* pStar3 = CCSprite::create("level_star.png");
              pStar3->setPosition(ccp(levelX+20, levelY-37));
              m_levels->addChild(pStar3);
          }
            earnedStarCount+=starCount;
          int no = i+1;
          for (int i = 0; i < 3; i++) {
              char noName[20];
              sprintf(noName, "level_no%d.png", no % 10);
              CCSprite* pNo = CCSprite::create(noName);
              pNo->setPosition(ccp(levelX+20-i*20, levelY+7));
              m_levels->addChild(pNo);
              no = no / 10;
          }
        } else {
            CCSprite* pLevelLock = CCSprite::create("level_locked.png");
            pLevelLock->setPosition(ccp(levelX, levelY));
            m_levels->addChild(pLevelLock);
        }
        lastScore = levelScore;
    }
    this->addChild(m_levels);
    m_levels->setPosition(ccp(m_levels->getPositionX()-m_currentPage*640, m_levels->getPositionY()));
    
    CCSprite* leftCover = CCSprite::create("level_left_cover.png");
    leftCover->setPosition(ccp(33, 452));
    this->addChild(leftCover);
    
    CCSprite* rightCover = CCSprite::create("level_right_cover.png");
    rightCover->setPosition(ccp(610, 452));
    this->addChild(rightCover);
    
    m_dots = CCLayer::create();
    refreshDotArea();
    this->addChild(m_dots);
    
    int curStar = earnedStarCount;
    int allStar = getAllStarCount();
    
    CCSprite* pProgress = CCSprite::create("level_progress.png");
    pProgress->setPosition(ccp(59, 792));
    pProgress->setAnchorPoint(ccp(0,0.5));
    pProgress->setScaleX(((float)(curStar+10))/((float)(allStar+10)));
    this->addChild(pProgress);
    
    int length = getNumberLength(allStar);
    CCSprite* pSpliter = CCSprite::create("level_slash.png");
    pSpliter->setPosition(ccp(320, 792));
    this->addChild(pSpliter);
    
    int start1 = 300;
    int start2 = 320+length*20;
    for (int i = 0; i < length; i++) {
        int c = curStar % 10;
        int a = allStar % 10;
        char num[20];
        
        sprintf(num, "level_no%d.png", c);
        CCSprite* pNum1 = CCSprite::create(num);
        pNum1->setPosition(ccp(start1-i*20, 792));
        this->addChild(pNum1);
        
        sprintf(num, "level_no%d.png", a);
        CCSprite* pNum2 = CCSprite::create(num);
        pNum2->setPosition(ccp(start2-i*20, 792));
        this->addChild(pNum2);
        curStar = curStar / 10;
        allStar = allStar / 10;
    }

    return true;
}

int LevelLayer::getEarnedStarCount() {
    if (m_mode != CLASSIC_MODE && m_mode != MUSHROOM_MODE) {
        return 0;
    }
    int cur = 0;
    for (int i = 0; i < CLASSIC_LEVEL_NO; i++) {
        char keyName[50];
        if (m_mode == CLASSIC_MODE) {
            sprintf(keyName, "classic_level_highscore_%d", i+1);
        } else if (m_mode == MUSHROOM_MODE) {
            sprintf(keyName, "mushroom_level_highscore_%d", i+1);
        }
        int levelScore = LOAD_INTEGER(keyName, 0);
        cur+=getLevelStarCount(i, levelScore);
    }
    return cur;
}

int LevelLayer::getAllStarCount() {
    if (m_mode == CLASSIC_MODE) {
        return 3 * CLASSIC_LEVEL_NO;
    } else if (m_mode == MUSHROOM_MODE) {
        return 3 * MUSHROOM_LEVEL_NO;
    } else {
        return 0;
    }
}

int LevelLayer::getLevelNo() {
    if (m_mode == CLASSIC_MODE) {
        return CLASSIC_LEVEL_NO;
    } else if (m_mode == MUSHROOM_MODE) {
        return MUSHROOM_LEVEL_NO;
    } else {
        return 0;
    }
}
int LevelLayer::getPageLevelNo() {
    if (m_mode == CLASSIC_MODE) {
        return CLASSIC_LEVEL_PAGE_NO;
    } else if (m_mode == MUSHROOM_MODE) {
        return MUSHROOM_LEVEL_PAGE_NO;
    } else {
        return 0;
    }
}
int LevelLayer::getPageColumnNo() {
    if (m_mode == CLASSIC_MODE) {
        return CLASSIC_LEVEL_COLUMN_NO;
    } else if (m_mode == MUSHROOM_MODE) {
        return MUSHROOM_LEVEL_COLUMN_NO;
    } else {
        return 0;
    }
}

int LevelLayer::getPageCount() {
    if (CLASSIC_LEVEL_NO % CLASSIC_LEVEL_PAGE_NO == 0) {
        return CLASSIC_LEVEL_NO / CLASSIC_LEVEL_PAGE_NO;
    } else {
        return CLASSIC_LEVEL_NO / CLASSIC_LEVEL_PAGE_NO + 1;
    }
}

int LevelLayer::getHishcore(int level) {
    if (m_mode == CLASSIC_MODE) {
        char keyName[50];
        sprintf(keyName, "classic_level_highscore_%d", level+1);
        return LOAD_INTEGER(keyName, 0);
    } else if (m_mode == MUSHROOM_MODE) {
        char keyName[50];
        sprintf(keyName, "mushroom_level_highscore_%d", level+1);
        return LOAD_INTEGER(keyName, 0);
    } else {
        return 0;
    }
}

int LevelLayer::getLevelStarCount(int level, int levelScore) {
    if (m_mode == CLASSIC_MODE) {
        int tileCount = 0;
        for (int i = 0; i < TILE_NO; i++) {
            for (int j = 0; j < TILE_NO; j++) {
                if (cLevelMaps[level][i][j] != 0) {
                    tileCount++;
                }
            }
        }
        int maxScore = tileCount*20+cLevelTimes[level]*50+tileCount*(tileCount-2)*10/8;
        if (levelScore > maxScore*0.65) {
            return 3;
        } else if (levelScore > maxScore*0.4) {
            return 2;
        } else if (levelScore > 0) {
            return 1;
        } else {
            return 0;
        }
    } else if (m_mode == MUSHROOM_MODE) {
        int tileCount = mMapPairCount[level]*2;
        int maxScore = tileCount*20+(mLevelTimes[level]*10/mGenerateDelays[level])+mLevelTimes[level]*50+tileCount*(tileCount-2)*10/8;
        if (levelScore > maxScore*0.65) {
            return 3;
        } else if (levelScore > maxScore*0.4) {
            return 2;
        } else if (levelScore > 0) {
            return 1;
        } else {
            return 0;
        }
    } else {
        return 0;
    }
}

void LevelLayer::refreshDotArea() {
    m_dots->removeAllChildrenWithCleanup(true);
    int page_no = getPageCount();
    int dot_start = (640-8*page_no-15*(page_no-1))/2+4;
    for (int i = 0; i < page_no; i++) {
        CCSprite* dot = CCSprite::create("white_dot.png");
        dot->setPosition(ccp(dot_start+i*(8+15), 150));
        m_dots->addChild(dot);
        if (i != m_currentPage) {
            dot->setOpacity(128);
        }
    }
}

void LevelLayer::backGameCallback(CCObject* pSender)
{
  CCTransitionFade* transition = CCTransitionFade::create(0.3, MenuLayer::scene());
  CCDirector::sharedDirector()->pushScene(transition);
}

void LevelLayer::keyBackClicked() {
    backGameCallback(NULL);
}

bool LevelLayer::ccTouchBegan(CCTouch *touch, CCEvent *event) {
    CCRect rect(0, 112, 640, 575);
    if (rect.containsPoint(touch->getLocation())) {
        m_beginPos = touch->getLocation();
        return true;
    }
    return false;
}

void LevelLayer::ccTouchMoved(cocos2d::CCTouch *touch, cocos2d::CCEvent *event) {
    CCPoint touchLoc = touch->getLocation();
    float moveX = touchLoc.x - m_beginPos.x;
    
    CCPoint curPos = m_levels->getPosition();
    CCPoint nextPos = ccp(curPos.x+moveX, curPos.y);
    m_levels->setPosition(nextPos);
    m_beginPos = touchLoc;
}

void LevelLayer::ccTouchEnded(cocos2d::CCTouch *touch, cocos2d::CCEvent *event) {
    int maxX = 0, minX = - 640 * (getPageCount()-1);
    int pointX = (int)(m_levels->getPositionX());
    if (pointX > maxX) {
        m_levels->runAction(CCMoveTo::create(0.2f, ccp(maxX, m_levels->getPositionY())));
        m_currentPage = 0;
        return;
    }
    if (pointX < minX) {
        m_levels->runAction(CCMoveTo::create(0.2f, ccp(minX, m_levels->getPositionY())));
        m_currentPage = getPageCount() - 1;
        return;
    }
    int remain = abs(pointX)%640;
    int other = 640 - remain;
    int desX = remain > other ? -(abs(pointX) + other) : -(abs(pointX)-remain);
    m_levels->runAction(CCMoveTo::create(0.2f, ccp(desX, m_levels->getPositionY())));
    m_currentPage = abs(desX)/640; 
    refreshDotArea();
}

void LevelLayer::onTouchEnded(cocos2d::CCTouch *touch, cocos2d::CCEvent *event) {
    ccTouchEnded(touch, event);
}

void LevelLayer::moveContainer(float x) {
    m_levels->setPosition(ccp(m_levels->getPositionX()+x, m_levels->getPositionY()));
}

void LevelLayer::onClick(LevelSprite *sprite) {
    CCTransitionFade* transition = CCTransitionFade::create(0.6, GameLayer::scene(m_mode, sprite->getLevel()));
    CCDirector::sharedDirector()->pushScene(transition);  
}

LevelSprite::LevelSprite(int level, LevelTouchListener* listener)
    : m_level(level), m_listener(listener){
}

LevelSprite* LevelSprite::create(const char *file,
    int level, LevelTouchListener *listener) {
    LevelSprite *sprite = new LevelSprite(level, listener);

    if (sprite && sprite->initWithFile(file)) {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return NULL;
}

bool LevelSprite::ccTouchBegan(cocos2d::CCTouch *touch, cocos2d::CCEvent *event) {
    bool bRet = false;
    
    if (containsTouchLocation(touch) && isVisible()) {
        beginPos = touch->getLocation();
        curPos = beginPos;
        bRet = true;
    }
    return bRet;
}

void LevelSprite::ccTouchMoved(cocos2d::CCTouch *touch, cocos2d::CCEvent *event) {
    if (m_listener != NULL) {
        CCPoint touchLoc = touch->getLocation();
        float moveX = touchLoc.x - curPos.x;
        m_listener->moveContainer(moveX);
        curPos = touchLoc;
    }
}

void LevelSprite::ccTouchEnded(cocos2d::CCTouch *touch, cocos2d::CCEvent *event) {
    CCPoint endPos = touch->getLocation();;

    if (m_listener != NULL) {
        if (abs(endPos.x-beginPos.x) < this->getContentSize().width) {
            m_listener->onClick(this);
        } else {
            m_listener->onTouchEnded(touch, event);
        }
    }
}
