/*
 * GameLayer.h
 *
 *  Created on: Sep 29, 2012
 *      Author: liuyaxin
 */

#ifndef GAMELAYER_H_
#define GAMELAYER_H_
#include "cocos2d.h"
#include "Common.h"
#include "TouchableSprite.h"
#include <vector>

class TileSprite;
typedef TileSprite* PTileSprite;

struct Coordinate {
    int x;
    int y;
};

enum Direction {
    LEFT, RIGHT, UP, DOWN, UNKNOWN
};

enum GameStatus {
    READY, RUNNING, PAUSED, WIN, LOSE
};

class TileTouchListener {
public:
    virtual void ccTouchBegan(TileSprite* sprite, CCTouch *touch, CCEvent *event) = 0;
    virtual void ccTouchMoved(TileSprite* sprite, CCTouch *touch, CCEvent *event) = 0;
    virtual void ccTouchEnded(TileSprite* sprite, CCTouch *touch, CCEvent *event) = 0;
};

class GameLayer : public cocos2d::CCLayer, public TileTouchListener {
public:
    GameLayer(int mode = CLASSIC_MODE, int level = 0) : m_mode(mode), m_level(level) {}
    virtual bool init();

    static cocos2d::CCScene* scene(int mode, int level);

    void soundOnCallback(CCObject* pSender);
    void soundOffCallback(CCObject* pSender);
    void pauseCallback(CCObject* pSender);
    void tipCallback(CCObject* pSender);
    void rerangeCallback(CCObject* pSender);
    void timeCallback(CCObject* pSender);
    void returnCallback(CCObject* pSender);
    void replayCallback(CCObject* pSender);
    void resumeCallback(CCObject* pSender);
    void nextCallback(CCObject* pSender);
    void playCallback(CCObject* pSender);
    void particleCallback(CCObject* pSender);
    void timeStepCallback(CCTime dt);
    void gainItemCallback(CCObject* pSender);
    virtual void ccTouchBegan(TileSprite* sprite, CCTouch *touch, CCEvent *event);
    virtual void ccTouchMoved(TileSprite* sprite, CCTouch *touch, CCEvent *event);
    virtual void ccTouchEnded(TileSprite* sprite, CCTouch *touch, CCEvent *event);
    virtual void keyBackClicked();
    virtual void keyMenuClicked();
    
    virtual void onEnter();
    virtual void onExit();

    static GameLayer* create(int mode, int level);
    void setLevel(int level) { m_level = level; }
    int getLevel() { return m_level; }
    void setMode(int mode) { m_mode = mode; }
    int getMode() { return m_mode; }

    CREATE_FUNC(GameLayer);

private:
    void rerangeMap();
    bool isDeathLock();
    bool checkWin();
    bool canLinkup(PTileSprite firstTile, PTileSprite secondTile, std::vector<Coordinate>& finallyPath);
    std::vector<Coordinate> getNeighbours(Coordinate current, Coordinate exception);
    std::vector<std::vector<Coordinate> > getRankedPaths(std::vector<std::vector<Coordinate> > paths, Coordinate end);
    bool isAdjacent(Coordinate first, Coordinate second);
    int getTurning(std::vector<Coordinate> path);
    int getTurning(std::vector<Coordinate> path, std::vector<Coordinate>& corners);
    bool isEqual(Coordinate first, Coordinate second);
    Direction getDirection(Coordinate first, Coordinate second);
    void runPath(const std::vector<Coordinate>& path);
    void gainItem(TileSprite* sprite);
    void generateRandomTile();
    void addTile(int tileIndex, Coordinate pos);

private:
    void initItemToolbar();
    void refreshItemNumber();
    int getLevelLength();
    int getMaxLevel();
    int getStarNo();
    int getHighscore();
    void saveHighscore();
    void showReadyStart();
    void showPauseLayer();
    void showResultLayer();
    void cleanFloatLayer();
    int getMapWidth();
    int getMapHeight(int mapWidth);
    float getLevelTime();
    int getTransformType();
    int getGenerateDelay();
    int getLineWidth(int line);
    int getMapTileValue(int i, int j);
    int getTileBorder(int mapWidth);
    float getTileScale(int mapWidth);
    void getStartXY(int mapWidth, int& start_x, int& start_y);
    std::vector<Coordinate> getNewCorners(Coordinate start, Coordinate end, std::vector<Coordinate> corners);
    bool isBlank(Coordinate first, Coordinate second);
    bool moveEnd(Coordinate start, Coordinate& end);
    int getDistance(Coordinate first, Coordinate second);
    void showCombo(int count, PTileSprite sprite);
    void transformMap(int type, PTileSprite first, PTileSprite second);
    void transformMap1(PTileSprite first, PTileSprite second);
    void transformMap2(PTileSprite first, PTileSprite second);
    void transformMap3(PTileSprite first, PTileSprite second);
    void transformMap4(PTileSprite first, PTileSprite second);
    void transformMap5(PTileSprite first, PTileSprite second);
    void transformMap6(PTileSprite first, PTileSprite second);
    void transformMap7(PTileSprite first, PTileSprite second);
    void transformMap8(PTileSprite first, PTileSprite second);
    void transformMap9(PTileSprite first, PTileSprite second);
    void transformMap10(PTileSprite first, PTileSprite second);
    void hideAllTiles();
    void showAllTiles();
    void moveAllTiles();
    void moveLeft(PTileSprite sprite, int end);
    void moveRight(PTileSprite sprite, int end);
    void moveTop(PTileSprite sprite, int end);
    void moveBottom(PTileSprite sprite, int end);
    void moveOut(PTileSprite sprite);
    void moveIn(PTileSprite sprite);
    void generateRandomMap(int min, int width, int height);
    void generateMushroomMap();
    void generateClassicMap();
    std::vector<int> getAvailableIndexes();
    void clearTileMap();
    void drawMap();
    int getEndlessTransformType();
    int getEndlessGenerateDelay();
    int getTileCount();
    void checkDeathAndRerange(bool showOrMove, bool forceRerange);

private:
    int m_tileMap[TILE_NO][TILE_NO];
    PTileSprite m_tileSpriteMap[TILE_NO][TILE_NO];
    cocos2d::CCSprite* m_progress;
    cocos2d::CCSprite* m_progressSmall;
    cocos2d::CCMenuItemImage* m_soundOnItem;
    cocos2d::CCMenuItemImage* m_soundOffItem;
    PTileSprite m_firstTile;
    std::vector<PTileSprite> availablePair;
    CCLayer* m_tipNumber;
    CCLayer* m_rerangeNumber;
    CCLayer* m_timeNumber;
    CCLayer* m_floatLayer;
    CCLayer* m_comboLayer;
    int m_level;
    int m_score;
    float m_time;
    float m_max_time;
    float m_mushroom_time;
    int m_two_star_score;
    int m_three_star_score;
    int m_mode;
    int m_start_x;
    int m_start_y;
    int m_tile_border;
    int m_transform_type;
    float m_tile_scale;
    float m_doublehit_time;
    float m_max_doublehit_time;
    int m_doublehit_count;
    int m_map_width;
    int m_map_height;
    int m_generate_delay;
    GameStatus m_status;
};

class TileSprite : public TouchableSprite {
public:
    TileSprite(int type, int x, int y, TileTouchListener *listener);

    static TileSprite* create(const char* file, int type, int x, int y, TileTouchListener* listener);

    void setXY(int x, int y) { m_x = x; m_y = y; }
    int getX() { return m_x; };
    int getY() { return m_y; };
    int getType() { return m_type; }
    void setDestPos(CCPoint destPos) { m_destPos = destPos; }
    const CCPoint& getDestPos() const { return m_destPos; }

    virtual bool ccTouchBegan(CCTouch *touch, CCEvent *event);
    virtual void ccTouchMoved(CCTouch *touch, CCEvent *event);
    virtual void ccTouchEnded(CCTouch *touch, CCEvent *event);
private:
    int m_type;
    int m_x;
    int m_y;
    CCPoint m_destPos;
    TileTouchListener* m_listener;
};

#endif /* GAMELAYER_H_ */
