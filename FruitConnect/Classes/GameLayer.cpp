/*
 * GameLayer.cpp
 *
 *  Created on: Sep 29, 2012
 *      Author: liuyaxin
 */

#include "GameLayer.h"
#include "LevelLayer.h"
#include "CommonFunctions.h"
#include "MenuLayer.h"
#include "ParticleWhiteSun.h"
#include "ClassicLevelData.h"
#include "MushroomLevelData.h"
#include "EndlessLevelData.h"
#include "SimpleAudioEngine.h"

using namespace cocos2d;

CCScene* GameLayer::scene(int mode, int level)
{
  // 'scene' is an autorelease object
  CCScene *scene = CCScene::create();

  // 'layer' is an autorelease object
  GameLayer *layer = GameLayer::create(mode, level);

  layer->setLevel(level);
  layer->setMode(mode);
    layer->setTag(LAYER_GAME);
  // add layer as a child to scene
  scene->addChild(layer);
    scene->setTag(SCENE_GAME);

  // return the scene
  return scene;
}

GameLayer* GameLayer::create(int mode, int level) {
  GameLayer* pRet = new GameLayer(mode, level);
  if (pRet && pRet->init()) {
      pRet->autorelease();
      return pRet;
  } else {
      CC_SAFE_DELETE(pRet);
      return NULL;
  }
}

// on "init" you need to initialize your instance
bool GameLayer::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !CCLayer::init() )
    {
        return false;
    }
    this->setKeypadEnabled(true);
    m_status = READY;

    CCSize size = CCDirector::sharedDirector()->getWinSize();

    CCSprite* pBackground = CCSprite::create("background.png");
    pBackground->setPosition(ccp(size.width/2, size.height/2));
    this->addChild(pBackground);
    
    CCSprite* pLeftTree = CCSprite::create("left_tree.png");
    pLeftTree->setPosition(ccp(137, 680));
    this->addChild(pLeftTree);
    
    CCSprite* pGameBox = CCSprite::create("game_box.png");
    pGameBox->setPosition(ccp(size.width/2, 468));
    this->addChild(pGameBox);
    
    CCSprite* pBoxFooter = CCSprite::create("box_footer.png");
    pBoxFooter->setPosition(ccp(size.width/2, 40));
    this->addChild(pBoxFooter);
    
    initItemToolbar();

    m_progress = CCSprite::create("progress.png");
    m_progress->setAnchorPoint(ccp(0,0.5));
    m_progress->setPosition(ccp(72, 792));
    this->addChild(m_progress);
    
    m_progressSmall = CCSprite::create("progress_small.png");
    m_progressSmall->setAnchorPoint(ccp(0,0.5));
    m_progressSmall->setPosition(ccp(534, 792));
    this->addChild(m_progressSmall);
    m_progressSmall->setScaleX(0);

    m_soundOnItem = CCMenuItemImage::create(
        "music_on.png", "music_on.png", this, menu_selector(GameLayer::soundOnCallback));
    m_soundOnItem->setPosition(ccp(610, 930));
    m_soundOnItem->setVisible(isMusicOn());

    m_soundOffItem = CCMenuItemImage::create(
        "music_off.png", "music_off.png", this, menu_selector(GameLayer::soundOffCallback));
    m_soundOffItem->setPosition(ccp(610, 930));
    m_soundOffItem->setVisible(!isMusicOn());

    CCMenuItemImage *pPauseItem = CCMenuItemImage::create(
        "pause_button.png", "pause_button.png", this, menu_selector(GameLayer::pauseCallback));
    pPauseItem->setPosition(ccp(30, 928));

    CCMenu* pMenu = CCMenu::create(m_soundOnItem, m_soundOffItem, pPauseItem, NULL);
    pMenu->setPosition(CCPointZero);
    this->addChild(pMenu);
    
    for (int i = 0; i < TILE_NO; i++) {
        for (int j = 0; j < TILE_NO; j++) {
            m_tileMap[i][j] = 0;
        }
    }
    int mapWidth = 0;
    if (m_mode == ENDLESS_MODE) {
        generateRandomMap(16, 6, 8);
        mapWidth = 6;
    } else if (m_mode == MUSHROOM_MODE) {
        generateMushroomMap();
        mapWidth = mMapWidth[m_level];
    } else if (m_mode == CLASSIC_MODE) {
        generateClassicMap();
        mapWidth = getMapWidth();
    }
    int tileCount = getTileCount();
    m_tile_border = getTileBorder(mapWidth);
    m_tile_scale = getTileScale(mapWidth);
    getStartXY(mapWidth, m_start_x, m_start_y);
    
    m_map_width = mapWidth;
    m_map_height = getMapHeight(m_map_width);
    
    drawMap();

    m_firstTile = NULL;
    checkDeathAndRerange(true, true);
    m_score = 0;
    m_max_time = getLevelTime();
    m_time = m_max_time;
    m_max_doublehit_time = 3;
    m_doublehit_time = 0;
    m_doublehit_count = 0;
    m_mushroom_time = 0;
    m_transform_type = getTransformType();
    m_generate_delay = getGenerateDelay();
    
    m_floatLayer = CCLayer::create();
    this->addChild(m_floatLayer, 100);
    
    m_comboLayer = CCLayer::create();
    this->addChild(m_comboLayer);
    if (m_mode == ENDLESS_MODE) {
        int high_score = getHighscore();
        int max_score = m_max_time * 100;
        m_three_star_score = max_score > high_score ? max_score : high_score;
    } else if (m_mode == CLASSIC_MODE) {
        int max_score = tileCount*20+m_max_time*50+tileCount*(tileCount-2)*10/8;
        m_two_star_score = max_score*0.4;
        m_three_star_score = max_score*0.65;
    } else if (m_mode == MUSHROOM_MODE) {
        tileCount += (m_max_time/m_generate_delay);
        int max_score = tileCount*20+m_max_time*50+tileCount*(tileCount-2)*10/8;
        m_two_star_score = max_score*0.4;
        m_three_star_score = max_score*0.65;
    }

    return true;
}


void GameLayer::generateClassicMap() {
    for (int i = 0; i < TILE_NO; i++) {
        for (int j = 0; j < TILE_NO; j++) {
            m_tileMap[i][j] = cLevelMaps[m_level][i][j];
        }
    }
}

void GameLayer::moveLeft(PTileSprite sprite, int end) {
    for (int i = sprite->getX(); i < end; i++) {
        CCPoint destPos = ccp(m_start_x + i * m_tile_border, m_start_y - sprite->getY() * m_tile_border);
        m_tileSpriteMap[sprite->getY()][i] = m_tileSpriteMap[sprite->getY()][i+1];
        if (m_tileSpriteMap[sprite->getY()][i] != NULL) {
            m_tileSpriteMap[sprite->getY()][i]->setXY(i, sprite->getY());
            m_tileSpriteMap[sprite->getY()][i]->setDestPos(destPos);
        }
    }
    m_tileSpriteMap[sprite->getY()][end] = NULL;
}

void GameLayer::moveRight(PTileSprite sprite, int end) {
    for (int i = sprite->getX(); i > end; i--) {
        CCPoint destPos = ccp(m_start_x + i * m_tile_border, m_start_y - sprite->getY() * m_tile_border);
        m_tileSpriteMap[sprite->getY()][i] = m_tileSpriteMap[sprite->getY()][i-1];
        if (m_tileSpriteMap[sprite->getY()][i] != NULL) {
            m_tileSpriteMap[sprite->getY()][i]->setXY(i, sprite->getY());
            m_tileSpriteMap[sprite->getY()][i]->setDestPos(destPos);
        }
    }
    m_tileSpriteMap[sprite->getY()][end] = NULL;
}

void GameLayer::moveTop(PTileSprite sprite, int end) {
    for (int i = sprite->getY(); i < end; i++) {
        CCPoint destPos = ccp(m_start_x + sprite->getX() * m_tile_border, m_start_y - i * m_tile_border);
        m_tileSpriteMap[i][sprite->getX()] = m_tileSpriteMap[i+1][sprite->getX()];
        if (m_tileSpriteMap[i][sprite->getX()] != NULL) {
            m_tileSpriteMap[i][sprite->getX()]->setXY(sprite->getX(), i);
            m_tileSpriteMap[i][sprite->getX()]->setDestPos(destPos);
        }
    }
    m_tileSpriteMap[end][sprite->getX()] = NULL;
}

void GameLayer::moveBottom(PTileSprite sprite, int end) {
    for (int i = sprite->getY(); i > end; i--) {
        CCPoint destPos = ccp(m_start_x + sprite->getX() * m_tile_border, m_start_y - i * m_tile_border);
        m_tileSpriteMap[i][sprite->getX()] = m_tileSpriteMap[i-1][sprite->getX()];
        if (m_tileSpriteMap[i][sprite->getX()] != NULL) {
            m_tileSpriteMap[i][sprite->getX()]->setXY(sprite->getX(), i);
            m_tileSpriteMap[i][sprite->getX()]->setDestPos(destPos);
        }
    }
    m_tileSpriteMap[end][sprite->getX()] = NULL;
}

void GameLayer::moveOut(PTileSprite sprite) {
    int x = sprite->getX();
    int y = sprite->getY();
    
    if (x < m_map_width/2 && y < m_map_height/2) {
        if (x > y) {
            moveLeft(sprite, m_map_width/2-1);
        } else {
            moveTop(sprite, m_map_height/2-1);
        }
    } else if (x < m_map_width/2 && y >= m_map_height/2) {
        if (x > m_map_height-y-1) {
            moveLeft(sprite, m_map_width/2-1);
        } else {
            moveBottom(sprite, m_map_height/2);
        }
    } else if (x >= m_map_width/2 && y < m_map_height/2) {
        if (m_map_width-1-x > y) {
            moveRight(sprite, m_map_width/2);
        } else {
            moveTop(sprite, m_map_height/2-1);
        }
    } else if (x >= m_map_width/2 && y >= m_map_height/2) {
        if (m_map_width-1-x > m_map_height-1-y) {
            moveRight(sprite, m_map_width/2);
        } else {
            moveBottom(sprite, m_map_height/2);
        }
    }
}

void GameLayer::moveIn(PTileSprite sprite) {
    int x = sprite->getX();
    int y = sprite->getY();
    if (x < m_map_width/2 && y < m_map_height/2) {
        if (x > y) {
            moveBottom(sprite, 0);
        } else {
            moveRight(sprite, 0);
        }
    } else if (x < m_map_width/2 && y >= m_map_height/2) {
        if (x > m_map_height-y-1) {
            moveTop(sprite, m_map_height-1);
        } else {
            moveRight(sprite, 0);
        }
        
    } else if (x >= m_map_width/2 && y < m_map_height/2) {
        if (m_map_width-x-1 > y) {
            moveBottom(sprite, 0);
        } else {
            moveLeft(sprite, m_map_width-1);
        }
    } else if (x >= m_map_width/2 && y >= m_map_height/2) {
        if (m_map_width-x-1 > m_map_height-y-1) {
            moveTop(sprite, m_map_height-1);
        } else {
            moveLeft(sprite, m_map_width-1);
        }
    }
}

void GameLayer::clearTileMap() {
    for (int i = 0; i < TILE_NO; i++) {
        for (int j = 0; j < TILE_NO; j++) {
            m_tileMap[i][j] = 0;
        }
    }
}

void GameLayer::generateRandomMap(int min, int width, int height) {
    int max = width * height / 2;
    int pair_no = min + abs(rand()) % (max-min+1);
    std::vector<Coordinate> posList;
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            Coordinate pos;
            pos.x = i;
            pos.y = j;
            posList.push_back(pos);
        }
    }
    for (int i = 0; i < pair_no; i++) {
        int cellIndex = abs(rand()) % 35 + 1;
        int posIndex1 = abs(rand())%posList.size();
        Coordinate pos1 = posList[posIndex1];
        posList.erase(posList.begin()+posIndex1);
        m_tileMap[pos1.y][pos1.x] = cellIndex;
        
        int posIndex2 = abs(rand())%posList.size();
        Coordinate pos2 = posList[posIndex2];
        posList.erase(posList.begin()+posIndex2);
        m_tileMap[pos2.y][pos2.x] = cellIndex;
    }
}

void GameLayer::checkDeathAndRerange(bool showOrMove, bool forceRerange) {
    bool reranged = false;
    if (forceRerange) {
        rerangeMap();
        reranged = true;
    }
    while (isDeathLock()) {
        rerangeMap();
        reranged = true;
    }
    if (showOrMove) {
        showAllTiles();
    } else {
        if (reranged) {
            moveAllTiles();
        }
    }
}

int GameLayer::getTileCount() {
    int tileCount = 0;
    for (int i = 0; i < TILE_NO; i++) {
        for (int j = 0; j < TILE_NO; j++) {
            if (m_tileMap[i][j] != 0) {
                tileCount++;
            }
        }
    }
    return tileCount;
}

void GameLayer::generateMushroomMap() {
    int width = mMapWidth[m_level];
    int height = getMapHeight(width);
    int pair_no = mMapPairCount[m_level];
    std::vector<Coordinate> posList;
    std::vector<int> indexList;
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            Coordinate pos;
            pos.x = i;
            pos.y = j;
            posList.push_back(pos);
        }
    }
    for (int i = 0; i < pair_no; i++) {
        if (indexList.size() <= 0) {
            indexList = getAvailableIndexes();
        }
        int cellIndex = indexList[indexList.size()-1];
        indexList.pop_back();
        int posIndex1 = abs(rand())%posList.size();
        Coordinate pos1 = posList[posIndex1];
        posList.erase(posList.begin()+posIndex1);
        m_tileMap[pos1.y][pos1.x] = cellIndex;
        
        int posIndex2 = abs(rand())%posList.size();
        Coordinate pos2 = posList[posIndex2];
        posList.erase(posList.begin()+posIndex2);
        m_tileMap[pos2.y][pos2.x] = cellIndex;
    }
}

std::vector<int> GameLayer::getAvailableIndexes() {
    std::vector<int> indexes;
    for (int i = mMapIndexMapRange[m_level]; i >= mMapIndexMinRange[m_level]; i--) {
        indexes.push_back(i);
    }
    return indexes;
}

void GameLayer::drawMap() {
    for (int i = 0; i < TILE_NO; i++) {
        for (int j = 0; j < TILE_NO; j++) {
            if (m_tileMap[i][j] != 0) {
                CCPoint destPos = ccp(m_start_x + j * m_tile_border, m_start_y - i * m_tile_border);
                char tileName[20];
                if (m_tileMap[i][j] > 0) {
                    sprintf(tileName, "cell%d.png", m_tileMap[i][j]);
                } else {
                    sprintf(tileName, "item%d.png", -m_tileMap[i][j]);
                }
                m_tileSpriteMap[i][j] = TileSprite::create(tileName, m_tileMap[i][j], j, i, this);
                m_tileSpriteMap[i][j]->setPosition(destPos);
                m_tileSpriteMap[i][j]->setDestPos(destPos);
                m_tileSpriteMap[i][j]->setScale(0.4*m_tile_scale);
                m_tileSpriteMap[i][j]->setOpacity(0);
                this->addChild(m_tileSpriteMap[i][j]);
            } else {
                m_tileSpriteMap[i][j] = NULL;
            }
        }
    }
}

int GameLayer::getEndlessTransformType() {
    if (m_time >= m_max_time*2/3) {
        if (abs(rand())%6 == 0) {
            return abs(rand())%10+1;
        }
    } else if (m_time >= m_max_time/3) {
        if (abs(rand())%4 == 0) {
            return abs(rand())%10+1;
        }
    } else {
        if (abs(rand())%2 == 0) {
            return abs(rand())%10+1;
        }
    }
    return 0;
}

int GameLayer::getEndlessGenerateDelay() {
    if (m_time >= m_max_time*2/3) {
        return 7;
    } else if (m_time >= m_max_time/3) {
        return 5;
    } else {
        return 3;
    }
}

int GameLayer::getMapTileValue(int i, int j) {
    if (m_mode == CLASSIC_MODE) {
        return cLevelMaps[m_level][i][j];
    } else {
        return 0;
    }
}

int GameLayer::getMapWidth() {
    int width = 0;
    for (int i = 0; i < TILE_NO; i++) {
        int lineWith = getLineWidth(i);
        if (lineWith > width) {
            width = lineWith;
        }
    }
    if (width <= 4) {
        width = 4;
    } else if (width <= 6) {
        width = 6;
    } else if (width <= 8) {
        width = 8;
    }
    return width;
}

int GameLayer::getMapHeight(int mapWidth) {
    if (mapWidth == 4) {
        return 6;
    } else if (mapWidth == 6) {
        return 8;
    } else if (mapWidth == 8) {
        return 10;
    } else {
        return 0;
    }
}

float GameLayer::getLevelTime() {
    if (m_mode == CLASSIC_MODE) {
        return cLevelTimes[m_level];
    } else if (m_mode == MUSHROOM_MODE) {
        return mLevelTimes[m_level];
    } else if (m_mode == ENDLESS_MODE) {
        return eLevelTimes[m_level];
    }
    return 0;
}

int GameLayer::getTransformType() {
    if (m_mode == CLASSIC_MODE) {
        return cMoveTypes[m_level];
    } else if (m_mode == MUSHROOM_MODE) {
        return mMoveTypes[m_level];
    }
    return 0;
}

int GameLayer::getGenerateDelay() {
    if (m_mode == MUSHROOM_MODE) {
        return mGenerateDelays[m_level];
    } else if (m_mode == ENDLESS_MODE) {
        return eGenerateDelays[m_level];
    } else {
        return 0;
    }
}

int GameLayer::getLineWidth(int line) {
    int left = TILE_NO-1, right = 0;
    for (int i = 0; i < TILE_NO; i++) {
        if (m_tileMap[line][i] != 0) {
            left = i;
            break;
        }
    }
    for (int i = TILE_NO-1; i >= 0; i--) {
        if (m_tileMap[line][i] != 0) {
            right = i;
            break;
        }
    }
    if (left <= right) {
        return right-left+1;
    } else {
        return 0;
    }
}

int GameLayer::getTileBorder(int mapWidth) {
    if (mapWidth == 4) {
        return 100;
    } else if (mapWidth == 6) {
        return 80;
    } else if (mapWidth == 8) {
        return 60;
    } else {
        return 0;
    }
}

float GameLayer::getTileScale(int mapWidth) {
    if (mapWidth == 4) {
        return 1.0f;
    } else if (mapWidth == 6) {
        return 0.8f;
    } else if (mapWidth == 8) {
        return 0.6f;
    } else {
        return 0;
    }
}

void GameLayer::getStartXY(int mapWidth, int &start_x, int &start_y) {
    if (mapWidth == 4) {
        start_x = 170;
        start_y = 680;
    } else if (mapWidth == 6) {
        start_x = 120;
        start_y = 690;
    } else if (mapWidth == 8) {
        start_x = 110;
        start_y = 700;
    } else {
        start_x = 0;
        start_y = 0;
    }
}

void GameLayer::initItemToolbar() {
    CCSprite* item_toolbar = CCSprite::create("items_toolbar.png");
    item_toolbar->setPosition(ccp(320, 865));
    this->addChild(item_toolbar);
    
    CCSprite* item_bg1 = CCSprite::create("item_bg.png");
    item_bg1->setPosition(ccp(330, 865));
    this->addChild(item_bg1);
    
    CCSprite* item_bg2 = CCSprite::create("item_bg.png");
    item_bg2->setPosition(ccp(420, 865));
    this->addChild(item_bg2);

    CCSprite* item_bg3 = CCSprite::create("item_bg.png");
    item_bg3->setPosition(ccp(510, 865));
    this->addChild(item_bg3);
    
    CCMenuItemImage *pTipItem = CCMenuItemImage::create("tip_item.png", "tip_item.png", this, menu_selector(GameLayer::tipCallback));
    pTipItem->setPosition(ccp(330, 865));
    pTipItem->setScale(0.75f);
    
    CCMenuItemImage *pRerangeItem = CCMenuItemImage::create("rerange_item.png", "rerange_item.png", this, menu_selector(GameLayer::rerangeCallback));
    pRerangeItem->setPosition(ccp(420, 865));
    pRerangeItem->setScale(0.75f);
    
    CCMenuItemImage *pTimeItem = CCMenuItemImage::create("time_item.png", "time_item.png", this, menu_selector(GameLayer::timeCallback));
    pTimeItem->setPosition(ccp(510, 865));
    pTimeItem->setScale(0.75f);
    
    CCMenu* pMenu = CCMenu::create(pTipItem, pRerangeItem, pTimeItem, NULL);
    pMenu->setPosition(CCPointZero);
    this->addChild(pMenu);

    m_tipNumber = CCLayer::create();
    this->addChild(m_tipNumber);
    
    m_rerangeNumber = CCLayer::create();
    this->addChild(m_rerangeNumber);
    
    m_timeNumber = CCLayer::create();
    this->addChild(m_timeNumber);
    
    refreshItemNumber();
    
    if (m_mode == CLASSIC_MODE) {
        CCSprite* title = CCSprite::create(getPngPath("game_title_classic.png")->getCString());
        title->setPosition(ccp(188, 874));
        this->addChild(title);
        
        CCSprite* level = CCSprite::create("game_level.png");
        level->setPosition(ccp(155, 840));
        this->addChild(level);
        int levelNo = m_level+1;
        int length = getLevelLength();
        for (int i = 0; i < length; i++) {
            int no = levelNo%10;
            CCSprite* noSprite = CCSprite::create("game_level_number.png", CCRect(135-no*15, 0, 15, 22));
            noSprite->setPosition(ccp(237-i*15,840));
            this->addChild(noSprite);
            levelNo /= 10;
        }
    } else if (m_mode == MUSHROOM_MODE) {
        CCSprite* title = CCSprite::create(getPngPath("game_title_mushroom.png")->getCString());
        title->setPosition(ccp(190, 874));
        this->addChild(title);
        
        CCSprite* level = CCSprite::create("game_level.png");
        level->setPosition(ccp(155, 840));
        this->addChild(level);
        int levelNo = m_level+1;
        int length = getLevelLength();
        for (int i = 0; i < length; i++) {
            int no = levelNo%10;
            CCSprite* noSprite = CCSprite::create("game_level_number.png", CCRect(135-no*15, 0, 15, 22));
            noSprite->setPosition(ccp(237-i*15,840));
            this->addChild(noSprite);
            levelNo /= 10;
        }
    } else if (m_mode == ENDLESS_MODE) {
        CCSprite* title = CCSprite::create(getPngPath("game_title_endless.png")->getCString());
        title->setPosition(ccp(185, 864));
        this->addChild(title);
    }
}

int GameLayer::getLevelLength() {
    if (m_mode == CLASSIC_MODE) {
        return getNumberLength(CLASSIC_LEVEL_NO);
    } else if (m_mode == MUSHROOM_MODE) {
        return getNumberLength(MUSHROOM_LEVEL_NO);
    } else {
        return 0;
    }
}

int GameLayer::getMaxLevel() {
    if (m_mode == CLASSIC_MODE) {
        return CLASSIC_LEVEL_NO;
    } else if (m_mode == MUSHROOM_MODE) {
        return MUSHROOM_LEVEL_NO;
    } else {
        return 0;
    }
}

void GameLayer::refreshItemNumber() {
    m_tipNumber->removeAllChildrenWithCleanup(true);
    int tipItemNumber = getTipItemCount();
    int initX = 358, initY = 840;
    do {
        int no = tipItemNumber % 10;
        CCSprite* noSprite = CCSprite::create("item_number.png", CCRect(12*no, 0, 12, 15));
        noSprite->setPosition(ccp(initX, initY));
        m_tipNumber->addChild(noSprite);
        initX-=12;
        tipItemNumber = tipItemNumber / 10;
    } while (tipItemNumber > 0);
    
    m_rerangeNumber->removeAllChildrenWithCleanup(true);
    int rerangeItemNumber = getRerangeItemCount();
    initX = 448;
    do {
        int no = rerangeItemNumber % 10;
        CCSprite* noSprite = CCSprite::create("item_number.png", CCRect(12*no, 0, 12, 15));
        noSprite->setPosition(ccp(initX, initY));
        m_rerangeNumber->addChild(noSprite);
        initX-=12;
        rerangeItemNumber = rerangeItemNumber / 10;
    } while (rerangeItemNumber > 0);
    
    m_timeNumber->removeAllChildrenWithCleanup(true);
    int timeItemNumber = getTimeItemCount();
    initX = 538;
    do {
        int no = timeItemNumber % 10;
        CCSprite* noSprite = CCSprite::create("item_number.png", CCRect(12*no, 0, 12, 15));
        noSprite->setPosition(ccp(initX, initY));
        m_rerangeNumber->addChild(noSprite);
        initX-=12;
        timeItemNumber = timeItemNumber / 10;
    } while (timeItemNumber > 0);
}

void GameLayer::keyBackClicked() {
    pauseCallback(NULL);
}

void GameLayer::keyMenuClicked() {
    callbackMenu();
}

void GameLayer::rerangeMap() {
  int cellCount = 0, i, j;
  std::vector<Coordinate> cellPos;
  for (i = 0; i < TILE_NO; i++) {
      for (j = 0; j < TILE_NO; j++) {
          if (m_tileSpriteMap[i][j] != NULL) {
              cellCount++;
              Coordinate pos;
              pos.x = j;
              pos.y = i;
              cellPos.push_back(pos);
          }
      }
  }
  if (cellCount <= 0) {
      return;
  }
  srand(time(0));
  for (i = 0; i < cellPos.size(); i++) {
      int change = rand() % cellPos.size();
      Coordinate first = cellPos[i];
      Coordinate second = cellPos[change];
      PTileSprite tmp = m_tileSpriteMap[first.y][first.x];
      CCPoint tmpPos = m_tileSpriteMap[first.y][first.x]->getDestPos();
      m_tileSpriteMap[first.y][first.x]->setDestPos(m_tileSpriteMap[second.y][second.x]->getDestPos());
      m_tileSpriteMap[second.y][second.x]->setDestPos(tmpPos);
      m_tileSpriteMap[first.y][first.x]->setXY(second.x, second.y);
      m_tileSpriteMap[second.y][second.x]->setXY(first.x, first.y);
      m_tileSpriteMap[first.y][first.x] = m_tileSpriteMap[second.y][second.x];
      m_tileSpriteMap[second.y][second.x] = tmp;
  }
}

bool GameLayer::checkWin() {
    for (int i = 0; i < TILE_NO; i++) {
        for (int j = 0; j < TILE_NO; j++) {
            if (m_tileSpriteMap[i][j] != NULL) {
                return false;
            }
        }
    }
    return true;
}

bool GameLayer::isDeathLock() {
    availablePair.clear();
    std::map<int, std::vector<PTileSprite> > map;
    for (int i = 0; i < TILE_NO; i++) {
        for (int j = 0; j < TILE_NO; j++) {
            if (m_tileSpriteMap[i][j] != NULL) {
                PTileSprite tile = m_tileSpriteMap[i][j];
                std::map<int, std::vector<PTileSprite> >::iterator iter = map.find(tile->getType());
                if (iter != map.end()) {
                    iter->second.push_back(m_tileSpriteMap[i][j]);
                } else {
                    std::vector<PTileSprite> vector;
                    vector.push_back(m_tileSpriteMap[i][j]);
                    map.insert(std::pair<int, std::vector<PTileSprite> >(tile->getType(), vector));
                }
            }
        }
    }
    if (map.size() == 0) {
        return false;
    }
    for (std::map<int, std::vector<PTileSprite> >::iterator iter = map.begin();
         iter != map.end();iter++) {
        const std::vector<PTileSprite>& vector = iter->second;
        if ((vector.size() % 2) == 1) {
            return false;
        }
        for (int i = 0; i < vector.size(); i++) {
            for (int j = i+1; j < vector.size(); j++) {
                std::vector<Coordinate> path;
                if (canLinkup(vector[i], vector[j], path)) {
                    availablePair.push_back(vector[i]);
                    availablePair.push_back(vector[j]);
                    return false;
                }
            }
        }
    }
    return true;
}

bool GameLayer::canLinkup(PTileSprite firstTile, PTileSprite secondTile, std::vector<Coordinate>& finalPath) {
    if (firstTile->getType() == secondTile->getType()) {
        std::vector<std::vector<Coordinate> > paths;
        Coordinate begin;
        begin.x = firstTile->getX();
        begin.y = firstTile->getY();
        Coordinate end;
        end.x = secondTile->getX();
        end.y = secondTile->getY();
        std::vector<Coordinate> path;
        path.push_back(begin);
        paths.push_back(path);
        for (;;) {
            if (paths.size() <= 0) {
                return false;
            }
            std::vector<Coordinate> lastPath = paths[paths.size()-1];
            paths.pop_back();
            if (lastPath.size() <= 0) {
                continue;
            }
            if (isAdjacent(lastPath[lastPath.size()-1], end)) {
                lastPath.push_back(end);
                if (getTurning(lastPath) < 3) {
                    finalPath = lastPath;
                    return true;
                }
            } else {
                Coordinate lastPoint = lastPath[lastPath.size()-1];
                Coordinate exception = lastPoint;
                if (lastPath.size() > 2) {
                    exception = lastPath[lastPath.size()-2];
                }
                std::vector<Coordinate> neighbours = getNeighbours(lastPoint, exception);
                std::vector<std::vector<Coordinate> > availablePaths;
                for (int i = 0; i < neighbours.size(); i++) {
                    std::vector<Coordinate> aPath = lastPath;
                    aPath.push_back(neighbours[i]);
                    if (getTurning(aPath) > 2) continue;
                    availablePaths.push_back(aPath);
                }
                std::vector<std::vector<Coordinate> > ranked = getRankedPaths(availablePaths, end);
                for (int i = 0; i < ranked.size(); i++) {
                    paths.push_back(ranked[i]);
                }
            }
            
        }
    }
    return false;
}

std::vector<std::vector<Coordinate> > GameLayer::getRankedPaths(std::vector<std::vector<Coordinate> > paths, Coordinate end) {
    std::vector<std::vector<Coordinate> > ranked;
    while (paths.size() > 0) {
        double max = 0;
        int maxPos = 0;
        for (int i = 0; i < paths.size(); i++) {
            const std::vector<Coordinate>& path = paths[i];
            if (path.size() <= 0) {
                continue;
            }
            const Coordinate& last = path[path.size()-1];
            int turning = getTurning(paths[i]);
            if (last.x != end.x && last.y != end.y) {
                turning += 2;
            } else {
                turning += 1;
            }
            double distance = sqrt(pow(last.x-end.x, 2)+pow(last.y-end.y, 2));
            double score = turning * 100 + distance * 0.01;
            if (score > max) {
                max = score;
                maxPos = i;
            }
        }
        ranked.push_back(paths[maxPos]);
        paths.erase(paths.begin()+maxPos);
    }
    return ranked;
}

std::vector<Coordinate> GameLayer::getNeighbours(Coordinate current, Coordinate exception) {
    std::vector<Coordinate> neighbours;
    if (current.x > 0) {
        Coordinate left;
        left.x = current.x - 1;
        left.y = current.y;
        if (!isEqual(left, exception)) {
            if (left.y < 0 || left.y > TILE_NO-1 || m_tileSpriteMap[left.y][left.x] == NULL) {
                neighbours.push_back(left);
            }
        }
    }
    if (current.x == 0) {
        Coordinate left;
        left.x = current.x -1;
        left.y = current.y;
        neighbours.push_back(left);
    }
    if (current.x < TILE_NO - 1) {
        Coordinate right;
        right.x = current.x + 1;
        right.y = current.y;
        if (!isEqual(right, exception)) {
            if (right.y < 0 || right.y > TILE_NO-1 || m_tileSpriteMap[right.y][right.x] == NULL) {
                neighbours.push_back(right);
            }
        }
    }
    if (current.x == TILE_NO - 1) {
        Coordinate right;
        right.x = current.x+1;
        right.y = current.y;
        neighbours.push_back(right);
    }
    if (current.y > 0) {
        Coordinate up;
        up.x = current.x;
        up.y = current.y - 1;
        if (!isEqual(up, exception)) {
            if (up.x < 0 || up.x > TILE_NO-1 || m_tileSpriteMap[up.y][up.x] == NULL) {
                neighbours.push_back(up);
            }
        }
    }
    if (current.y == 0) {
        Coordinate up;
        up.x = current.x;
        up.y = current.y - 1;
        neighbours.push_back(up);
    }
    if (current.y < TILE_NO - 1) {
        Coordinate down;
        down.x = current.x;
        down.y = current.y + 1;
        if (!isEqual(down, exception)) {
            if (down.x < 0 || down.x > TILE_NO-1 || m_tileSpriteMap[down.y][down.x] == NULL) {
                neighbours.push_back(down);
            }
        }
    }
    if (current.y == TILE_NO - 1 ) {
        Coordinate down;
        down.x = current.x;
        down.y = current.y + 1;
        neighbours.push_back(down);
    }
    return neighbours;
}

bool GameLayer::isAdjacent(Coordinate first, Coordinate second) {
    if ((abs(first.x-second.x) + abs(first.y-second.y)) == 1) {
        return true;
    } else {
        return false;
    }
}

bool GameLayer::isEqual(Coordinate first, Coordinate second) {
    return (first.x == second.x && first.y == second. y);
}

Direction GameLayer::getDirection(Coordinate first, Coordinate second) {
    if (first.x == second.x) {
        if (second.y > first.y) {
            return UP;
        } else if (second.y < first.y) {
            return DOWN;
        }
    } else if (first.y == second.y) {
        if (second.x > first.x) {
            return RIGHT;
        } else if (second.x < first.x) {
            return LEFT;
        }
    }
    return UNKNOWN;
}

int GameLayer::getTurning(std::vector<Coordinate> path) {
    if (path.size() <= 2) {
        return 0;
    }
    int turn = 0;
    Direction last = getDirection(path[0], path[1]);
    for (int i = 1; i < path.size() - 1; i++) {
        Direction current = getDirection(path[i], path[i+1]);
        if (last != current) {
            turn++;
        }
        last = current;
    }
    return turn;
}

int GameLayer::getTurning(std::vector<Coordinate> path, std::vector<Coordinate> &corners) {
    if (path.size() <= 2) {
        return 0;
    }
    int turn = 0;
    Direction last = getDirection(path[0], path[1]);
    for (int i = 1; i < path.size() - 1; i++) {
        Direction current = getDirection(path[i], path[i+1]);
        if (last != current) {
            corners.push_back(path[i]);
            turn++;
        }
        last = current;
    }
    return turn;
}

void GameLayer::onEnter() {
    showReadyStart();
    CCLayer::onEnter();
}

void GameLayer::onExit() {
    CCLayer::onExit();
}

void GameLayer::showReadyStart() {
    cleanFloatLayer();
    CCSprite* bg = CCSprite::create("float_bg.png");
    bg->setPosition(ccp(320, 460));
//    bg->setVisible(false);
    bg->setOpacity(128);
    m_floatLayer->addChild(bg,10);
    bg->runAction(CCSequence::create(CCShow::create(), CCDelayTime::create(2.0), CCHide::create(), NULL));
    
    CCSprite* ready = CCSprite::create("float_ready.png");
    ready->setPosition(ccp(320, 460));
    ready->setVisible(false);
    m_floatLayer->addChild(ready,10);
    ready->runAction(CCSequence::create(CCShow::create(), CCFadeIn::create(0.2f), CCDelayTime::create(0.6), CCFadeOut::create(0.2f), NULL));
    
    CCSprite* start = CCSprite::create("float_start.png");
    start->setPosition(ccp(320, 460));
    start->setVisible(false);
    m_floatLayer->addChild(start,10);
    start->runAction(CCSequence::create(CCDelayTime::create(1.0f), CCShow::create(), CCFadeIn::create(0.2f), CCDelayTime::create(0.6), CCFadeOut::create(0.2f), CCCallFuncN::create(this, callfuncN_selector(GameLayer::playCallback)), NULL));
    playMusic("ready_go.wav");
    showGameAd();
}

void GameLayer::showPauseLayer() {
    cleanFloatLayer();
    CCSprite* bg = CCSprite::create("float_bg.png");
    bg->setPosition(ccp(320, 460));
    bg->setOpacity(128);
    m_floatLayer->addChild(bg);
    
    CCSprite* title = CCSprite::create("game_title_paused.png");
    title->setPosition(ccp(320, 520));
    m_floatLayer->addChild(title);
    title->runAction(CCFadeIn::create(0.2f));
    
    CCMenuItemImage* pReturn = CCMenuItemImage::create("game_button_return.png", "game_button_return.png", this, menu_selector(GameLayer::returnCallback));
    pReturn->setPosition(ccp(205, 400));
    CCMenuItemImage* pReplay = CCMenuItemImage::create("game_button_replay.png", "game_button_replay.png", this, menu_selector(GameLayer::replayCallback));
    pReplay->setPosition(ccp(320, 400));
    CCMenuItemImage* pResume = CCMenuItemImage::create("game_button_resume.png", "game_button_resume.png", this, menu_selector(GameLayer::resumeCallback));
    pResume->setPosition(ccp(435, 400));
    
    CCMenu* pMenu = CCMenu::create(pReturn, pReplay, pResume, NULL);
    pMenu->setPosition(CCPointZero);
    m_floatLayer->addChild(pMenu);
    pReturn->runAction(CCFadeIn::create(0.2f));
    pReplay->runAction(CCFadeIn::create(0.2f));
    pResume->runAction(CCFadeIn::create(0.2f));
    
    showGamePauseAd();
}

void GameLayer::showResultLayer() {
    cleanFloatLayer();
    if (m_status == LOSE && m_mode != ENDLESS_MODE) {
        CCSprite* bg = CCSprite::create("float_bg.png");
        bg->setPosition(ccp(320, 460));
        bg->setOpacity(192);
        m_floatLayer->addChild(bg);
        
        CCSprite* title = CCSprite::create("game_title_failed.png");
        title->setPosition(ccp(320, 520));
        title->setOpacity(0);
        m_floatLayer->addChild(title);
        title->runAction(CCFadeIn::create(0.2f));
        
        CCMenuItemImage* pReturn = CCMenuItemImage::create("game_button_return.png", "game_button_return.png", this, menu_selector(GameLayer::returnCallback));
        pReturn->setOpacity(0);
        pReturn->setPosition(ccp(252, 400));
        CCMenuItemImage* pReplay = CCMenuItemImage::create("game_button_replay.png", "game_button_replay.png", this, menu_selector(GameLayer::replayCallback));
        pReplay->setOpacity(0);
        pReplay->setPosition(ccp(387, 400));
        CCMenu* pMenu = CCMenu::create(pReturn, pReplay, NULL);
        pMenu->setPosition(CCPointZero);
        m_floatLayer->addChild(pMenu);
        pReturn->runAction(CCFadeIn::create(0.2f));
        pReplay->runAction(CCFadeIn::create(0.2f));
    } else if (m_status == WIN || m_mode == ENDLESS_MODE) {
        CCSprite* bg = CCSprite::create("float_bg.png");
        bg->setPosition(ccp(320, 460));
        bg->setScaleY(1.8f);
        if (m_mode == ENDLESS_MODE) {
            bg->setOpacity(192);
        } else {
            bg->setOpacity(128);
        }
        m_floatLayer->addChild(bg);
        bool isLastLevel = m_level >= getMaxLevel()-1;
        
        CCMenuItemImage* pReturn = CCMenuItemImage::create("game_button_return.png", "game_button_return.png", this, menu_selector(GameLayer::returnCallback));
        pReturn->setOpacity(0);
        pReturn->setPosition(ccp(190, 260));
        CCMenuItemImage* pReplay = CCMenuItemImage::create("game_button_replay.png", "game_button_replay.png", this, menu_selector(GameLayer::replayCallback));
        pReplay->setOpacity(0);
        pReplay->setPosition(ccp(320, 260));
        pReturn->runAction(CCSequence::create(CCDelayTime::create(0.3f), CCFadeIn::create(0.2f), NULL));
        pReplay->runAction(CCSequence::create(CCDelayTime::create(0.3f), CCFadeIn::create(0.2f), NULL));
        
        if (m_mode == ENDLESS_MODE) {
            if (m_level == 0) {
                CCSprite* title = CCSprite::create("three_minutes_ended.png");
                title->setOpacity(0);
                title->setPosition(ccp(320, 665));
                m_floatLayer->addChild(title);
                title->runAction(CCSequence::create(CCDelayTime::create(0.3f), CCFadeIn::create(0.2f), NULL));
            } else if (m_level == 1) {
                CCSprite* title = CCSprite::create("six_minutes_ended.png");
                title->setOpacity(0);
                title->setPosition(ccp(320, 665));
                m_floatLayer->addChild(title);
                title->runAction(CCSequence::create(CCDelayTime::create(0.3f), CCFadeIn::create(0.2f), NULL));
            }
            
            pReturn->setPosition(ccp(252, 260));
            pReplay->setPosition(ccp(387, 260));
            
            CCMenu* pMenu = CCMenu::create(pReturn, pReplay, NULL);
            pMenu->setPosition(CCPointZero);
            m_floatLayer->addChild(pMenu);
        } else if (isLastLevel) {
            CCSprite* cong = CCSprite::create("game_congradulations.png");
            cong->setOpacity(0);
            cong->setPosition(ccp(320, 694));
            m_floatLayer->addChild(cong);
            cong->runAction(CCSequence::create(CCDelayTime::create(0.3f), CCFadeIn::create(0.2f), NULL));
            
            if (m_mode == CLASSIC_MODE) {
                CCSprite* cleared = CCSprite::create("game_classic_mode_cleared.png");
                cleared->setOpacity(0);
                cleared->setPosition(ccp(320, 645));
                m_floatLayer->addChild(cleared);
                cleared->runAction(CCSequence::create(CCDelayTime::create(0.3f), CCFadeIn::create(0.2f), NULL));
            } else if (m_mode == MUSHROOM_MODE) {
                CCSprite* cleared = CCSprite::create("game_mushroom_mode_cleared.png");
                cleared->setOpacity(0);
                cleared->setPosition(ccp(320, 645));
                m_floatLayer->addChild(cleared);
                cleared->runAction(CCSequence::create(CCDelayTime::create(0.3f), CCFadeIn::create(0.2f), NULL));
            } else if (m_mode == ENDLESS_MODE) {
                CCSprite* cleared = CCSprite::create("game_mushroom_mode_cleared.png");
                cleared->setOpacity(0);
                cleared->setPosition(ccp(320, 645));
                m_floatLayer->addChild(cleared);
                cleared->runAction(CCSequence::create(CCDelayTime::create(0.3f), CCFadeIn::create(0.2f), NULL));
            }
            pReturn->setPosition(ccp(252, 260));
            pReplay->setPosition(ccp(387, 260));
            
            CCMenu* pMenu = CCMenu::create(pReturn, pReplay, NULL);
            pMenu->setPosition(CCPointZero);
            m_floatLayer->addChild(pMenu);
        } else {
            CCSprite* title = CCSprite::create("game_title_cleared.png");
            title->setOpacity(0);
            title->setPosition(ccp(320, 665));
            m_floatLayer->addChild(title);
            title->runAction(CCSequence::create(CCDelayTime::create(0.3f), CCFadeIn::create(0.2f), NULL));
            
            CCMenuItemImage* pNext = CCMenuItemImage::create("game_button_next.png", "game_button_next.png", this, menu_selector(GameLayer::nextCallback));
            pNext->setOpacity(0);
            pNext->setPosition(ccp(450, 260));
            pNext->runAction(CCSequence::create(CCDelayTime::create(0.3f), CCFadeIn::create(0.2f), NULL));
            CCMenu* pMenu = CCMenu::create(pReturn, pReplay, pNext, NULL);
            pMenu->setPosition(CCPointZero);
            m_floatLayer->addChild(pMenu);
        }
        
        int starNo = getStarNo();
        CCSprite* leftDown = CCSprite::create("game_star_left_down.png");
        leftDown->setOpacity(0);
        leftDown->setPosition(ccp(168, 530));
        leftDown->setScale(0.8);
        m_floatLayer->addChild(leftDown);
        leftDown->runAction(CCSequence::create(CCDelayTime::create(0.3f), CCFadeIn::create(0.2f), NULL));
        
        CCSprite* middleDown = CCSprite::create("game_star_middle_down.png");
        middleDown->setOpacity(0);
        middleDown->setPosition(ccp(320, 550));
        middleDown->setScale(0.8);
        m_floatLayer->addChild(middleDown);
        middleDown->runAction(CCSequence::create(CCDelayTime::create(0.3f), CCFadeIn::create(0.2f), NULL));
        
        CCSprite* rightDown = CCSprite::create("game_star_right_down.png");
        rightDown->setOpacity(0);
        rightDown->setPosition(ccp(473, 530));
        rightDown->setScale(0.8);
        m_floatLayer->addChild(rightDown);
        rightDown->runAction(CCSequence::create(CCDelayTime::create(0.3f), CCFadeIn::create(0.2f), NULL));
        
        if (starNo > 0) {
            CCSprite* leftUp = CCSprite::create("game_star_left_up.png");
            leftUp->setOpacity(0);
            leftUp->setPosition(ccp(168, 530));
            m_floatLayer->addChild(leftUp);
            leftUp->runAction(CCSequence::create(CCDelayTime::create(1.0f), CCFadeIn::create(0.1f), NULL));
        }
        
        if (starNo > 1) {
            CCSprite* middleUp = CCSprite::create("game_star_middle_up.png");
            middleUp->setOpacity(0);
            middleUp->setPosition(ccp(320, 550));
            m_floatLayer->addChild(middleUp);
            middleUp->runAction(CCSequence::create(CCDelayTime::create(1.8f), CCFadeIn::create(0.1f), NULL));
        }
        
        if (starNo > 2) {
            CCSprite* rightUp = CCSprite::create("game_star_right_up.png");
            rightUp->setOpacity(0);
            rightUp->setPosition(ccp(473, 530));
            m_floatLayer->addChild(rightUp);
            rightUp->runAction(CCSequence::create(CCDelayTime::create(2.6f), CCFadeIn::create(0.1f), NULL));
        }
        if (m_score > getHighscore()) {
            CCSprite* highScore = CCSprite::create("game_new_highscore.png");
            highScore->setOpacity(0);
            highScore->setPosition(ccp(320, 340));
            m_floatLayer->addChild(highScore);
            highScore->runAction(CCSequence::create(CCDelayTime::create(4.4f), CCFadeIn::create(0.1f), NULL));
            CCSprite* highSign = CCSprite::create("game_highscore_sign.png");
            highSign->setOpacity(0);
            highSign->setPosition(ccp(547,390));
            m_floatLayer->addChild(highSign);
            highSign->runAction(CCSequence::create(CCDelayTime::create(5.2f), CCFadeIn::create(0.1f), NULL));
        } else {
            int high = getHighscore();
            int size_width = 90 + 20 + getNumberLength(high)*20;
            int bestX = (640-size_width)/2+45;
            int noStartX = 320+size_width/2-10;
            CCSprite* best = CCSprite::create("game_best.png");
            best->setOpacity(0);
            best->setPosition(ccp(bestX, 340));
            m_floatLayer->addChild(best);
            best->runAction(CCSequence::create(CCDelayTime::create(4.4f), CCFadeIn::create(0.1f), NULL));
            do {
                int no = high % 10;
                high = high / 10;
                CCSprite* noSprite = CCSprite::create("game_score_small_numbers.png", CCRect(20*no, 0, 20, 33));
                noSprite->setOpacity(0);
                noSprite->setPosition(ccp(noStartX, 340));
                m_floatLayer->addChild(noSprite);
                noSprite->runAction(CCSequence::create(CCDelayTime::create(4.4f), CCFadeIn::create(0.1f), NULL));
                noStartX -= 20;
            } while (high > 0);
        }
        int length = getNumberLength(m_score);
        int initX = 320+length*45/2-22;
        int initY = 410;
        int score = m_score;
        do {
            int no = score%10;
            score = score/10;
            CCSprite* noSprite = CCSprite::create("game_score_numbers.png", CCRect((9-no)*45, 0, 45, 64));
            noSprite->setOpacity(0);
            noSprite->setPosition(ccp(initX, initY));
            m_floatLayer->addChild(noSprite);
            noSprite->runAction(CCSequence::create(CCDelayTime::create(3.4f), CCFadeIn::create(0.1f), NULL));
            initX -= 45;
        } while (score > 0);
    }
    showGamePauseAd();
}

int GameLayer::getStarNo() {
    if (m_mode == ENDLESS_MODE) {
        if (m_score >= m_three_star_score) {
            return 3;
        } else if (m_score >= m_three_star_score*0.7) {
            return 2;
        } else if (m_score >= m_three_star_score*0.3) {
            return 1;
        } else {
            return 0;
        }
    } else {
        if (m_score >= m_three_star_score) {
            return 3;
        } else if (m_score >= m_two_star_score) {
            return 2;
        } else if (m_score > 0) {
            return 1;
        } else  {
            return 0;
        }
    }
}

int GameLayer::getHighscore() {
    char keyName[50];
    if (m_mode == CLASSIC_MODE) {
        sprintf(keyName, "classic_level_highscore_%d", m_level+1);
    } else if (m_mode == MUSHROOM_MODE) {
        sprintf(keyName, "mushroom_level_highscore_%d", m_level+1);
    } else if (m_mode == ENDLESS_MODE) {
        sprintf(keyName, "endless_level_highscore_%d", m_level+1);
    }
    int levelScore = LOAD_INTEGER(keyName, 0);
    return levelScore;
}

void GameLayer::saveHighscore() {
    int highscore = getHighscore();
    if (m_score > highscore) {
        char keyName[50];
        if (m_mode == CLASSIC_MODE) {
            sprintf(keyName, "classic_level_highscore_%d", m_level+1);
        } else if (m_mode == MUSHROOM_MODE) {
            sprintf(keyName, "mushroom_level_highscore_%d", m_level+1);
        } else if (m_mode == ENDLESS_MODE) {
            sprintf(keyName, "endless_level_highscore_%d", m_level+1);
        }
        SAVE_INTEGER(keyName, m_score);
        FLUSH_DATA;
    }
}

void GameLayer::cleanFloatLayer() {
    m_floatLayer->removeAllChildrenWithCleanup(true);
}

void GameLayer::returnCallback(cocos2d::CCObject *pSender) {
    if (m_mode == CLASSIC_MODE) {
        CCTransitionFade* transition = CCTransitionFade::create(0.6, LevelLayer::scene(CLASSIC_MODE));
        CCDirector::sharedDirector()->pushScene(transition);
    } else if (m_mode == MUSHROOM_MODE) {
        CCTransitionFade* transition = CCTransitionFade::create(0.6, LevelLayer::scene(MUSHROOM_MODE));
        CCDirector::sharedDirector()->pushScene(transition);
    } else if (m_mode == ENDLESS_MODE) {
        CCTransitionFade* transition = CCTransitionFade::create(0.6, MenuLayer::scene());
        CCDirector::sharedDirector()->pushScene(transition);
    }
}

void GameLayer::replayCallback(cocos2d::CCObject *pSender) {
    CCTransitionFade* transition = CCTransitionFade::create(0.6, GameLayer::scene(m_mode, m_level));
    CCDirector::sharedDirector()->pushScene(transition);
}

void GameLayer::nextCallback(cocos2d::CCObject *pSender) {
    if (m_mode == CLASSIC_MODE && m_mode < CLASSIC_LEVEL_NO-1) {
        CCTransitionFade* transition = CCTransitionFade::create(0.6, GameLayer::scene(m_mode, m_level+1));
        CCDirector::sharedDirector()->pushScene(transition);
    } else if (m_mode == MUSHROOM_MODE && m_mode < MUSHROOM_LEVEL_NO-1) {
        CCTransitionFade* transition = CCTransitionFade::create(0.6, GameLayer::scene(m_mode, m_level+1));
        CCDirector::sharedDirector()->pushScene(transition);
    }
}

void GameLayer::resumeCallback(cocos2d::CCObject *pSender) {
    if (m_status == PAUSED) {
        showReadyStart();
        showAllTiles();
    }
}

void GameLayer::soundOnCallback(CCObject* pSender) {
    m_soundOnItem->setVisible(false);
    m_soundOffItem->setVisible(true);
    setMusicOn(false);
}

void GameLayer::soundOffCallback(CCObject* pSender) {
    m_soundOffItem->setVisible(false);
    m_soundOnItem->setVisible(true);
    setMusicOn(true);
}
void GameLayer::pauseCallback(CCObject* pSender) {
    if (m_status == RUNNING) {
        m_status = PAUSED;
        this->unschedule(schedule_selector(GameLayer::timeStepCallback));
        hideAllTiles();
        showPauseLayer();
        m_doublehit_time = 0;
        m_doublehit_count = 0;
        m_progressSmall->setScaleX(m_doublehit_time/m_max_doublehit_time);
    }
}

void GameLayer::hideAllTiles() {
    for (int i = 0; i < TILE_NO; i++) {
        for (int j = 0; j < TILE_NO; j++) {
            if (m_tileSpriteMap[i][j] != NULL) {
                m_tileSpriteMap[i][j]->stopAllActions();
                m_tileSpriteMap[i][j]->runAction(CCFadeOut::create(0.2f));
            }
        }
    }
}

void GameLayer::showAllTiles() {
    for (int i = 0; i < TILE_NO; i++) {
        for (int j = 0; j < TILE_NO; j++) {
            if (m_tileSpriteMap[i][j] != NULL) {
                float delay = (float)(abs(rand())%3)/(float)10;
                m_tileSpriteMap[i][j]->setScale(m_tile_scale);
                m_tileSpriteMap[i][j]->setPosition(m_tileSpriteMap[i][j]->getDestPos());
                m_tileSpriteMap[i][j]->runAction(CCSequence::create(CCDelayTime::create(delay),
                                                                    CCFadeIn::create(0.1f),
                                                                    CCScaleTo::create(0.2, 1.0f*m_tile_scale),
                                                                    CCScaleTo::create(0.2, 0.8f*m_tile_scale),
                                                                    CCScaleTo::create(0.2, 1.0f*m_tile_scale),
                                                                    CCScaleTo::create(0.2, 0.9f*m_tile_scale),
                                                                    CCScaleTo::create(0.2, 1.0f*m_tile_scale), NULL));
            }
        }
    }
}

void GameLayer::moveAllTiles() {
    for (int i = 0; i < TILE_NO; i++) {
        for (int j = 0; j < TILE_NO; j++) {
            if (m_tileSpriteMap[i][j] != NULL) {
                m_tileSpriteMap[i][j]->runAction(CCMoveTo::create(0.3, m_tileSpriteMap[i][j]->getDestPos()));
            }
        }
    }
}

void GameLayer::tipCallback(CCObject* pSender) {
    if (m_status != RUNNING) {
        return;
    }
    int tipNumber = getTipItemCount();
    if (tipNumber > 0) {
        saveTipItemCount(tipNumber-1);
        refreshItemNumber();
        CCMenuItemImage* tipItem = (CCMenuItemImage*)pSender;
        tipItem->runAction(CCSequence::createWithTwoActions(CCScaleTo::create(0.2, 0.9), CCScaleTo::create(0.2, 0.75)));
        for (int i = 0; i < availablePair.size(); i++) {
            PTileSprite sprite = availablePair[i];
            sprite->runAction(CCSequence::create(CCScaleTo::create(0.2, 1.2*m_tile_scale),
                                                 CCScaleTo::create(0.2, 0.9*m_tile_scale),
                                                 CCScaleTo::create(0.2, 1.1*m_tile_scale),
                                                 CCScaleTo::create(0.2, 0.9*m_tile_scale),
                                                 CCScaleTo::create(0.2, 1.1*m_tile_scale),
                                                 CCScaleTo::create(0.2, 1.0*m_tile_scale),
                                                 NULL));
        }
    }
    playMusic("press_button.wav");
}
void GameLayer::rerangeCallback(CCObject* pSender) {
    if (m_status != RUNNING) {
        return;
    }
    for (int i = 0; i < availablePair.size(); i++) {
        PTileSprite sprite = availablePair[i];
        sprite->setScale(m_tile_scale);
    }
    int rerangeNumber = getRerangeItemCount();
    if (rerangeNumber > 0) {
        saveRerangeItemCount(rerangeNumber-1);
        refreshItemNumber();
        CCMenuItemImage* rerangeItem = (CCMenuItemImage*)pSender;
        rerangeItem->runAction(CCSequence::createWithTwoActions(CCScaleTo::create(0.2, 0.9), CCScaleTo::create(0.2, 0.75)));
        checkDeathAndRerange(false, true);
    }
    playMusic("press_button.wav");
}

void GameLayer::timeCallback(CCObject* pSender) {
    if (m_status != RUNNING) {
        return;
    }
    int timeNumber = getTimeItemCount();
    if (timeNumber > 0) {
        saveTimeItemCount(timeNumber-1);
        refreshItemNumber();
        CCMenuItemImage* timeItem = (CCMenuItemImage*)pSender;
        timeItem->runAction(CCSequence::createWithTwoActions(CCScaleTo::create(0.2, 0.9), CCScaleTo::create(0.2, 0.75)));
        m_time += 10;
        if (m_time > m_max_time) {
            m_time = m_max_time;
        }
        m_progress->setScaleX(m_time/m_max_time);
        CCSprite* add_time = CCSprite::create("add_time.png");
        add_time->setPosition(ccp(490,792));
        this->addChild(add_time);
        add_time->runAction(CCSequence::create(CCMoveTo::create(0.5f, ccp(490, 802)), CCFadeOut::create(0.2f), NULL));
    }
    playMusic("press_button.wav");
}

void GameLayer::playCallback(cocos2d::CCObject *pSender) {
    cleanFloatLayer();
    m_status = RUNNING;
    this->schedule(schedule_selector(GameLayer::timeStepCallback), 0.1f);
}

void GameLayer::timeStepCallback(cocos2d::CCTime dt) {
    if (m_status == RUNNING) {
        m_time -= 0.1f;
        if (((int)(m_time*10)) == 30 || ((int)(m_time*10)) == 20 || ((int)(m_time*10)) == 10) {
            playMusic("last_time.wav");
        }
        m_progress->runAction(CCScaleTo::create(0.1, m_time/m_max_time, 1));
        m_doublehit_time -= 0.1f;
        if (m_doublehit_time < 0) {
            m_doublehit_time = 0;
        }
        if (m_doublehit_time == 0){
            m_doublehit_count = 0;
        }
        if (m_generate_delay > 0) {
            m_mushroom_time += 0.1f;
            if (m_mushroom_time > m_generate_delay) {
                generateRandomTile();
                m_mushroom_time = 0;
                if (m_mode == ENDLESS_MODE) {
                    m_generate_delay = getEndlessGenerateDelay();
                }
                checkDeathAndRerange(false, false);
            }
        }
        m_progressSmall->runAction(CCScaleTo::create(0.1, m_doublehit_time/m_max_doublehit_time, 1));
        if (m_time <= 0) {
            m_status = LOSE;
            this->unschedule(schedule_selector(GameLayer::timeStepCallback));
            if (m_firstTile != NULL) {
                m_firstTile->stopAllActions();
                m_firstTile->runAction(CCScaleTo::create(0.2, m_tile_scale));
            }
            for (int i = 0; i < availablePair.size(); i++) {
                availablePair[i]->stopAllActions();
            }
            showResultLayer();
            if (m_mode == ENDLESS_MODE) {
                playMusic("win.wav");
            } else {
                playMusic("lose.wav");
            }
        }
    }
}

void GameLayer::gainItemCallback(cocos2d::CCObject *pSender) {
    refreshItemNumber();
}

void GameLayer::gainItem(TileSprite *sprite) {
    CCPoint point = sprite->getPosition();
    if (sprite->getType() == -1) {
        saveTipItemCount(getTipItemCount()+1);
        CCSprite* item = CCSprite::create("tip_item.png");
        item->setPosition(point);
        item->setScale(0.5f);
        item->setOpacity(128);
        this->addChild(item);
        item->runAction(CCSequence::create(CCDelayTime::create(0.2f),
                                          CCMoveTo::create(0.6f, ccp(330,865)), CCFadeOut::create(0.2f),
                                          CCCallFuncN::create(this, callfuncN_selector(GameLayer::gainItemCallback)), NULL));
    } else if (sprite->getType() == -2) {
        saveRerangeItemCount(getRerangeItemCount()+1);
        CCSprite* item = CCSprite::create("rerange_item.png");
        item->setPosition(point);
        item->setScale(0.5f);
        item->setOpacity(128);
        this->addChild(item);
        item->runAction(CCSequence::create(CCDelayTime::create(0.2f),
                                           CCMoveTo::create(0.6f, ccp(420,865)), CCFadeOut::create(0.2f),
                                           CCCallFuncN::create(this, callfuncN_selector(GameLayer::gainItemCallback)), NULL));
    } else if (sprite->getType() == -3) {
        saveTimeItemCount(getTimeItemCount()+1);
        CCSprite* item = CCSprite::create("time_item.png");
        item->setPosition(point);
        item->setScale(0.5f);
        item->setOpacity(128);
        this->addChild(item);
        item->runAction(CCSequence::create(CCDelayTime::create(0.2f),
                                           CCMoveTo::create(0.6f, ccp(510,865)), CCFadeOut::create(0.2f),
                                           CCCallFuncN::create(this, callfuncN_selector(GameLayer::gainItemCallback)), NULL));
    }
}

void GameLayer::generateRandomTile() {
    int height = 0;
    if (m_map_width == 4) {
        height = 6;
    } else if (m_map_width == 6) {
        height = 8;
    } else if (m_map_width == 8) {
        height = 10;
    }
    std::vector<Coordinate> empty_pos;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < m_map_width; j++) {
            if (m_tileSpriteMap[i][j] == NULL) {
                Coordinate pos;
                pos.x = j;
                pos.y = i;
                empty_pos.push_back(pos);
            }
        }
    }
    if (empty_pos.size() <= 1) {
        return;
    }
    int index1 = abs(rand())%empty_pos.size();
    int index2 = index1;
    while (index2 == index1) {
        index2 = abs(rand())%empty_pos.size();
    }
    int tileIndex = abs(rand())%40;
    if (tileIndex > 0 && tileIndex < 36) {
        addTile(tileIndex, empty_pos[index1]);
        addTile(tileIndex, empty_pos[index2]);
    } else if (tileIndex >= 36 && tileIndex < 39) {
        addTile(35-tileIndex, empty_pos[index1]);
        addTile(35-tileIndex, empty_pos[index2]);
    }
    checkDeathAndRerange(false, false);
}

void GameLayer::addTile(int tileIndex, Coordinate pos) {
    CCPoint destPos = ccp(m_start_x+pos.x*m_tile_border, m_start_y-pos.y*m_tile_border);
    char tileName[20];
    if (tileIndex > 0) {
        sprintf(tileName, "cell%d.png", tileIndex);
    } else {
        sprintf(tileName, "item%d.png", -tileIndex);
    }
    m_tileSpriteMap[pos.y][pos.x] = TileSprite::create(tileName, tileIndex, pos.x, pos.y, this);
    m_tileSpriteMap[pos.y][pos.x]->setPosition(destPos);
    m_tileSpriteMap[pos.y][pos.x]->setDestPos(destPos);
    m_tileSpriteMap[pos.y][pos.x]->setScale(0.4*m_tile_scale);
    m_tileSpriteMap[pos.y][pos.x]->setOpacity(0);
    this->addChild(m_tileSpriteMap[pos.y][pos.x], 0);
    float delay = (float)(abs(rand())%3)/(float)10;
    m_tileSpriteMap[pos.y][pos.x]->runAction(CCSequence::create(CCDelayTime::create(delay),
                                                                CCFadeIn::create(0.1f),
                                                                CCScaleTo::create(0.2, 1.0f*m_tile_scale),
                                                                CCScaleTo::create(0.2, 0.8f*m_tile_scale),
                                                                CCScaleTo::create(0.2, 1.0f*m_tile_scale),
                                                                CCScaleTo::create(0.2, 0.9f*m_tile_scale),
                                                                CCScaleTo::create(0.2, 1.0f*m_tile_scale), NULL));
}

void GameLayer::ccTouchBegan(TileSprite* sprite, CCTouch *touch, CCEvent *event) {
}

void GameLayer::ccTouchMoved(TileSprite* sprite, CCTouch *touch, CCEvent *event) {
}

void GameLayer::ccTouchEnded(TileSprite* sprite, CCTouch *touch, CCEvent *event) {
    if (m_status != RUNNING) {
        return;
    }
  if (m_firstTile == NULL) {
      sprite->runAction(CCRepeatForever::create(CCSequence::createWithTwoActions(
          CCScaleTo::create(0.2f, m_tile_scale*0.8), CCScaleTo::create(0.1f, m_tile_scale))));
      m_firstTile = sprite;
      playMusic("first_tile.wav");
  } else {
      if (m_firstTile == sprite) {
//          sprite->stopAllActions();
//          sprite->setScale(1.0f*m_tile_scale);
//          m_firstTile = NULL;
      } else {
          std::vector<Coordinate> finalPath;
          if (canLinkup(m_firstTile, sprite, finalPath)) {
              if (sprite->getType() < 0) {
                  gainItem(sprite);
              }
              m_tileSpriteMap[m_firstTile->getY()][m_firstTile->getX()] = NULL;
              m_tileSpriteMap[sprite->getY()][sprite->getX()] = NULL;
              m_firstTile->runAction(CCSequence::create(CCFadeOut::create(0.2), CCHide::create(), NULL));
              sprite->runAction(CCSequence::create(CCFadeOut::create(0.2), CCHide::create(), NULL));
              runPath(finalPath);
              m_score += 40;
              if (m_doublehit_time > 0) {
                  m_score += m_doublehit_count*10;
                  m_doublehit_count++;
                  showCombo(m_doublehit_count, sprite);
              }
              m_doublehit_time = m_max_doublehit_time;
              playMusic("second_tile.wav");
              if (checkWin()) {
                  if (m_mode == ENDLESS_MODE) {
                      m_score += 1000;
                      clearTileMap();
                      generateRandomMap(16, 6, 8);
                      drawMap();
                      checkDeathAndRerange(true, false);
                      return;
                  } else {
                      playMusic("win.wav");
                      m_score += 50*m_time;
                      m_status = WIN;
                      this->unschedule(schedule_selector(GameLayer::timeStepCallback));
                      showResultLayer();
                      saveHighscore();
                      return;
                  }
              }
              if (m_mode == ENDLESS_MODE) {
                  m_transform_type = getEndlessTransformType();
              }
              if (m_transform_type != 0) {
                  m_tileSpriteMap[m_firstTile->getY()][m_firstTile->getX()] = m_firstTile;
                  m_tileSpriteMap[sprite->getY()][sprite->getX()] = sprite;
                  transformMap(m_transform_type, m_firstTile, sprite);
              }
              checkDeathAndRerange(false, false);
              m_firstTile = NULL;
          } else {
              m_firstTile->stopAllActions();
              m_firstTile->setScale(m_tile_scale);
              sprite->runAction(CCRepeatForever::create(CCSequence::createWithTwoActions(
                  CCScaleTo::create(0.2f, 0.8f*m_tile_scale), CCScaleTo::create(0.1f, m_tile_scale))));
              m_firstTile = sprite;
              playMusic("first_tile.wav");
          }
      }
  }
}

void GameLayer::runPath(const std::vector<Coordinate> &path) {
    CCPoint begin = ccp(m_start_x+path[0].x*m_tile_border, m_start_y-path[0].y*m_tile_border);
    
    CCParticleSystem* fire = CCParticleWhiteSun::create((path.size()-1)*50);
    fire->setPosition(begin);
    fire->setTexture(CCTextureCache::sharedTextureCache()->addImage("fire.png"));
    this->addChild(fire);
    
    std::vector<Coordinate> corners;
    int turning = getTurning(path, corners);
    if (turning == 2 && getDirection(path[0], corners[0]) == getDirection(path[path.size()-1], corners[1])) {
        corners = getNewCorners(path[0], path[path.size()-1], corners);
    }
    CCPoint end(m_start_x+path[path.size()-1].x*m_tile_border, m_start_y-path[path.size()-1].y*m_tile_border);
    if (turning == 0) {
        fire->runAction(CCSequence::create(CCMoveTo::create(0.3, end), CCCallFuncN::create(this, callfuncN_selector(GameLayer::particleCallback)), NULL));
    } else if (turning == 1 && corners.size() == 1) {
        CCPoint first(m_start_x+corners[0].x*m_tile_border, m_start_y-corners[0].y*m_tile_border);
        float dis1 = getDistance(path[0], corners[0]);
        float dis2 = getDistance(corners[0], path[path.size()-1]);
        float disA = dis1 + dis2;
        fire->runAction(CCSequence::create(CCMoveTo::create(0.3*dis1/disA, first), CCMoveTo::create(0.3*dis2/disA, end), CCCallFuncN::create(this, callfuncN_selector(GameLayer::particleCallback)), NULL));
    } else if (turning == 2 && corners.size() == 2) {
        CCPoint first(m_start_x+corners[0].x*m_tile_border, m_start_y-corners[0].y*m_tile_border);
        CCPoint second(m_start_x+corners[1].x*m_tile_border, m_start_y-corners[1].y*m_tile_border);
        float dis1 = getDistance(path[0], corners[0]);
        float dis2 = getDistance(corners[0], corners[1]);
        float dis3 = getDistance(corners[1], path[path.size()-1]);
        float disA = dis1+dis2+dis3;
        fire->runAction(CCSequence::create(CCMoveTo::create(0.3*dis1/disA, first), CCMoveTo::create(0.3*dis2/disA, second), CCMoveTo::create(0.3*dis3/disA, end), CCCallFuncN::create(this, callfuncN_selector(GameLayer::particleCallback)),  NULL));
    }
}

int GameLayer::getDistance(Coordinate first, Coordinate second) {
    return abs(first.x-second.x)+abs(first.y-second.y);
}

void GameLayer::showCombo(int count, PTileSprite sprite) {
    m_comboLayer->removeAllChildrenWithCleanup(true);
    CCPoint p = sprite->getPosition();
    CCSprite* combo = CCSprite::create("combo.png");
    combo->setPosition(ccp(p.x-15, p.y));
    m_comboLayer->addChild(combo);
    combo->runAction(CCSequence::create(CCDelayTime::create(0.2f), CCMoveTo::create(0.3, ccp(p.x-15, p.y+10)), CCFadeOut::create(0.2f), NULL));
    int len = getNumberLength(count);
    int startx = p.x + 25 + 5 + 6 + 13*(len-1);
    do {
        int no = count % 10;
        CCSprite* noSp = CCSprite::create("combo_numbers.png", CCRect(no*13, 0, 13, 20));
        noSp->setPosition(ccp(startx, p.y));
        m_comboLayer->addChild(noSp);
        noSp->runAction(CCSequence::create(CCDelayTime::create(0.2f), CCMoveTo::create(0.3, ccp(startx, p.y+10)), CCFadeOut::create(0.2f), NULL));
        startx -= 13;
        count = count / 10;
    } while (count > 0);
}

void GameLayer::transformMap(int type, PTileSprite first, PTileSprite second) {
    switch (type) {
        case 1:
            transformMap1(first, second);
            break;
        case 2:
            transformMap2(first, second);
            break;
        case 3:
            transformMap3(first, second);
            break;
        case 4:
            transformMap4(first, second);
            break;
        case 5:
            transformMap5(first, second);
            break;
        case 6:
            transformMap6(first, second);
            break;
        case 7:
            transformMap7(first, second);
            break;
        case 8:
            transformMap8(first, second);
            break;
        case 9:
            transformMap9(first, second);
            break;
        case 10:
            transformMap10(first, second);
            break;
        default:
            return;
    }
    checkDeathAndRerange(false, false);
    moveAllTiles();
}

void GameLayer::transformMap1(PTileSprite first, PTileSprite second) {
    moveLeft(first, m_map_width-1);
    moveLeft(second, m_map_width-1);
}

void GameLayer::transformMap2(PTileSprite first, PTileSprite second) {
    moveRight(first, 0);
    moveRight(second, 0);
}

void GameLayer::transformMap3(PTileSprite first, PTileSprite second) {
    moveTop(first, m_map_height-1);
    moveTop(second, m_map_height-1);
}

void GameLayer::transformMap4(PTileSprite first, PTileSprite second) {
    moveBottom(first, 0);
    moveBottom(second, 0);
}

void GameLayer::transformMap5(PTileSprite first, PTileSprite second) {
    moveOut(first);
    moveOut(second);
}

void GameLayer::transformMap6(PTileSprite first, PTileSprite second) {
    moveIn(first);
    moveIn(second);
}

void GameLayer::transformMap7(PTileSprite first, PTileSprite second) {
    if (first->getY() < m_map_height/2) {
        moveLeft(first, m_map_width-1);
    } else {
        moveRight(first, 0);
    }
    if (second->getY() < m_map_height/2) {
        moveLeft(second, m_map_width-1);
    } else {
        moveRight(second, 0);
    }
}

void GameLayer::transformMap8(PTileSprite first, PTileSprite second) {
    if (first->getX() < m_map_width/2) {
        moveTop(first, m_map_height-1);
    } else {
        moveBottom(first, 0);
    }
    if (second->getX() < m_map_width/2) {
        moveTop(second, m_map_height-1);
    } else {
        moveBottom(second, 0);
    }
}

void GameLayer::transformMap9(PTileSprite first, PTileSprite second) {
    if (first->getY() >= m_map_height/2) {
        moveLeft(first, m_map_width-1);
    } else {
        moveRight(first, 0);
    }
    if (second->getY() >= m_map_height/2) {
        moveLeft(second, m_map_width-1);
    } else {
        moveRight(second, 0);
    }
}

void GameLayer::transformMap10(PTileSprite first, PTileSprite second) {
    if (first->getX() >= m_map_width/2) {
        moveTop(first, m_map_height-1);
    } else {
        moveBottom(first, 0);
    }
    if (second->getX() >= m_map_width/2) {
        moveTop(second, m_map_height-1);
    } else {
        moveBottom(second, 0);
    }
}

std::vector<Coordinate> GameLayer::getNewCorners(Coordinate start, Coordinate end, std::vector<Coordinate> corners) {
    std::vector<Coordinate> newCorners = corners;
    Coordinate startEnd = corners[0];
    Coordinate endEnd = corners[1];
    while (true) {
        if (moveEnd(start, startEnd) && moveEnd(end, endEnd)) {
            if (isBlank(startEnd, endEnd)) {
                newCorners.clear();
                newCorners.push_back(startEnd);
                newCorners.push_back(endEnd);
            }
        } else {
            break;
        }
    }
    return newCorners;
}

bool GameLayer::moveEnd(Coordinate start, Coordinate& end) {
    if (start.x == end.x) {
        int factor = start.y > end.y ? 1 : -1;
        if (end.y + factor == start.y) {
            return false;
        }
        end.y = end.y + factor;
        return true;
    } else if (start.y == end.y) {
        int factor = start.x > end.x ? 1 : -1;
        if (end.x + factor == start.x) {
            return false;
        }
        end.x = end.x + factor;
        return true;
    } else {
        return false;
    }
}

bool GameLayer::isBlank(Coordinate first, Coordinate second) {
    if (first.x == second.x) {
        int factor = first.y > second.y ? -1 : 1;
        int differ = abs(first.y-second.y);
        for (int i = 0; i < differ; i++) {
            int y = first.y + factor*i;
            if (m_tileSpriteMap[y][first.x] != NULL) {
                return false;
            }
        }
        return true;
    } else if (first.y == second.y) {
        int factor = first.x > second.x ? -1 : 1;
        int differ = abs(first.x-second.x);
        for (int i = 0; i < differ; i++) {
            int x = first.x + factor*i;
            if (m_tileSpriteMap[first.y][x] != NULL) {
                return false;
            }
        }
        return true;
    } else {
        return false;
    }
}

void GameLayer::particleCallback(cocos2d::CCObject *pSender) {
    CCParticleSystem* fire = (CCParticleSystem*) pSender;
    fire->stopSystem();
}

TileSprite::TileSprite(int type, int x, int y, TileTouchListener *listener)
    : m_type(type), m_x(x), m_y(y), m_listener(listener) {
}

TileSprite* TileSprite::create(const char* file, int type, int x, int y, TileTouchListener *listener) {
    TileSprite *sprite = new TileSprite(type, x, y, listener);

    if (sprite && sprite->initWithFile(file)) {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return NULL;
}

bool TileSprite::ccTouchBegan(cocos2d::CCTouch *touch, cocos2d::CCEvent *event) {
    bool bRet = false;
    if (containsTouchLocation(touch) && isVisible()) {
        if (m_listener != NULL) {
            m_listener->ccTouchBegan(this, touch, event);
        }
        bRet = true;
    }
    return bRet;
}

void TileSprite::ccTouchMoved(cocos2d::CCTouch *touch, cocos2d::CCEvent *event) {
    if (m_listener != NULL) {
        m_listener->ccTouchMoved(this, touch, event);
    }
}

void TileSprite::ccTouchEnded(cocos2d::CCTouch *touch, cocos2d::CCEvent *event) {
    if (m_listener != NULL) {
        m_listener->ccTouchEnded(this, touch, event);
    }
}
